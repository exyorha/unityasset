#include <UnityAsset/SerializedAsset/SerializedAssetFile.h>

#include <UnityAsset/Streams/Stream.h>

#include <limits>

namespace UnityAsset {

    SerializedAssetFile::SerializedAssetFile() : assetVersion(21) {

    }

    SerializedAssetFile::~SerializedAssetFile() = default;

    SerializedAssetFile::SerializedAssetFile(SerializedAssetFile &&other) noexcept = default;

    SerializedAssetFile &SerializedAssetFile::operator =(SerializedAssetFile &&other) noexcept = default;

    SerializedAssetFile::SerializedAssetFile(Stream &&stream) : SerializedAssetFile() {
        stream.setByteOrder(Stream::ByteOrder::MostSignificantFirst);

        uint32_t metadataSizeNarrow, fileSizeNarrow;

        stream >> metadataSizeNarrow >> fileSizeNarrow;

        stream >> assetVersion;
        if(assetVersion != 21 && assetVersion != 22)
            throw std::runtime_error("SerializedAssetFile: unexpected file version");

        uint32_t dataOffsetNarrow;
        uint32_t fileFlags;
        stream >> dataOffsetNarrow >> fileFlags;

        if(fileFlags != 0)
            throw std::runtime_error("SerializedAssetFile: unexpected file flags");

        uint32_t metadataSize; // yes, this is supposed to be 32-bit
        uint64_t fileSize;
        uint64_t dataOffset;

        if(assetVersion >= 22) {
            if(metadataSizeNarrow != 0 || fileSizeNarrow != 0 || dataOffsetNarrow != 0)
                throw std::runtime_error("one of the narrow sizes is not zero in a version 22 offset");

            stream >> metadataSize >> fileSize >> dataOffset;

            uint64_t unknown;
            stream >> unknown;
            if(unknown != 0)
                throw std::runtime_error("the version 22 unknown field is not zero");
        } else {
            metadataSize = metadataSizeNarrow;
            fileSize = fileSizeNarrow;
            dataOffset = dataOffsetNarrow;
        }

        if(fileSize != stream.length())
            throw std::runtime_error("SerializedAssetFile: file size in the header is inconsistent with the stream length");

        auto metadataStream = stream.createView(stream.position(), metadataSize);
        metadataStream.setByteOrder(Stream::ByteOrder::LeastSignificantFirst); // MSB first if (flags & 1) == 1

        unityVersion = metadataStream.readNullTerminatedString();
        metadataStream >> platform;

        metadataStream >> typeTreeEnabled;

        int32_t typeCount;
        metadataStream >> typeCount;

        m_Types.reserve(typeCount);

        for(int32_t element = 0; element < typeCount; element++) {
            m_Types.emplace_back(metadataStream, false, typeTreeEnabled);
        }

        int32_t objectCount;
        metadataStream >> objectCount;

        m_Objects.reserve(objectCount);

        auto dataArea = stream.createView(dataOffset);

        for(int32_t element = 0; element < objectCount; element++) {
            m_Objects.emplace_back(metadataStream, dataArea, assetVersion >= 22);
        }

        int32_t scriptTypeCount;
        metadataStream >> scriptTypeCount;

        m_ScriptTypes.reserve(scriptTypeCount);

        for(int32_t element = 0; element < scriptTypeCount; element++) {
            m_ScriptTypes.emplace_back(metadataStream);
        }

        int32_t externalCount;
        metadataStream >> externalCount;

        m_Externals.reserve(externalCount);

        for(int32_t element = 0; element < externalCount; element++) {
            m_Externals.emplace_back(metadataStream);
        }

        int32_t refTypeCount;
        metadataStream >> refTypeCount;

        m_RefTypes.reserve(refTypeCount);

        for(int32_t element = 0; element < refTypeCount; element++) {
            m_RefTypes.emplace_back(metadataStream, true, typeTreeEnabled);
        }

        userInformation = metadataStream.readNullTerminatedString();

        if(metadataStream.position() != metadataStream.length()) {
            throw std::runtime_error("did not reach the end of metadata while parsing");
        }
    }

    void SerializedAssetFile::serialize(Stream &output) const {
        /*
         * Begin by serializing the metadata.
         */

        Stream metadataStream;
        metadataStream.setByteOrder(Stream::ByteOrder::LeastSignificantFirst);
        metadataStream.writeNullTerminatedString(unityVersion);
        metadataStream << platform;

        metadataStream << typeTreeEnabled;

        metadataStream << static_cast<int32_t>(m_Types.size());
        for(const auto &type: m_Types) {
            type.serialize(metadataStream);
        }

        size_t dataOffset = 0;

        metadataStream << static_cast<int32_t>(m_Objects.size());
        for(const auto &object: m_Objects) {
            metadataStream.alignPosition(4);
            metadataStream << object.m_PathID;

            if(assetVersion >= 22) {
                /*
                 * Write the full 64-bit offset
                 */
                metadataStream << static_cast<uint64_t>(dataOffset);
            } else {
                if(dataOffset > std::numeric_limits<int32_t>::max())
                    throw std::runtime_error("the object data offset exceeds that allowed for a version 21 offset");

                metadataStream << static_cast<uint32_t>(dataOffset);
            }

            if(object.objectData.length() > std::numeric_limits<int32_t>::max())
                throw std::runtime_error("the object data is too long: objects longer than 4GiB are not supported in any version of Unity");

            metadataStream << static_cast<uint32_t>(object.objectData.length());

            dataOffset += object.objectData.length();
            dataOffset = (dataOffset + 15) & ~15;

            metadataStream << object.typeIndex;
        }

        metadataStream << static_cast<int32_t>(m_ScriptTypes.size());
        for(const auto &scriptType: m_ScriptTypes) {
            scriptType.serialize(metadataStream);
        }

        metadataStream << static_cast<int32_t>(m_Externals.size());
        for(const auto &external: m_Externals) {
            external.serialize(metadataStream);
        }

        metadataStream << static_cast<int32_t>(m_RefTypes.size());
        for(const auto &refType: m_RefTypes) {
            refType.serialize(metadataStream);
        }

        metadataStream.writeNullTerminatedString(userInformation);

        output.setByteOrder(Stream::ByteOrder::MostSignificantFirst);

        if(metadataStream.length() > std::numeric_limits<int32_t>::max())
            throw std::runtime_error("the metadata stream is too long to represent");

        size_t fileSizeOffset;

        if(assetVersion >= 22) {
            /*
             * Dummy 32-bit values that are always zero:
             */

            output << static_cast<uint32_t>(0); // unused metadata size
            output << static_cast<uint32_t>(0); // narrow file size
        } else {
            fileSizeOffset = output.position();
            output << static_cast<uint32_t>(0); // file size - will be fixed later
        }

        output << assetVersion;

        size_t dataStartOffset;
        if(assetVersion >= 22) {
            /*
             * Dummy 32-bit data start offset that's always zero
             */
            output << static_cast<uint32_t>(0);
        } else {
            dataStartOffset = output.position();
            output << static_cast<uint32_t>(0); // data start offset - will be fixed later
        }

        output << static_cast<uint32_t>(0); // flags - always 0

        if(assetVersion >= 22) {
            /*
             * Metadata length - still 32-bit, but relocated here.
             */
            output << static_cast<uint32_t>(metadataStream.length());

            /*
            * Actual 64-bit lengths and offsets.
            */

            fileSizeOffset = output.position();
            output << static_cast<uint64_t>(0); // file size - will be fixed later

            dataStartOffset = output.position();
            output << static_cast<uint64_t>(0); // data start offset - will be fixed later

            output << static_cast<uint64_t>(0); // This field seems to always be zero
        }

        output.writeData(metadataStream.data(), metadataStream.length());

        if(output.position() < 4096)
            output.setPosition(4096);
        else
            output.alignPosition(16);

        size_t dataAreaOffset = output.position();
        output.setPosition(dataStartOffset);

        if(assetVersion >= 22) {

            output << static_cast<uint64_t>(dataAreaOffset);
        } else {
            if(dataAreaOffset > std::numeric_limits<int32_t>::max()) {
                throw std::logic_error("the asset is too long for the version 21 with the 32-bit sizes and offsets");
            }

            output << static_cast<uint32_t>(dataAreaOffset);
        }

        output.setPosition(dataAreaOffset);

        for(const auto &object: m_Objects) {
            output.alignPosition(16);
            output.writeData(object.objectData.data(), object.objectData.length());
        }

        auto fileSize = output.position();
        output.setPosition(fileSizeOffset);

        if(assetVersion >= 22) {
            output << static_cast<uint64_t>(fileSize);

        } else {
            if(fileSize > std::numeric_limits<int32_t>::max()) {
                throw std::logic_error("the asset is too long for the version 21 with the 32-bit sizes and offsets");
            }


            output << static_cast<uint32_t>(fileSize);
        }

        output.setPosition(fileSize);
    }
}
