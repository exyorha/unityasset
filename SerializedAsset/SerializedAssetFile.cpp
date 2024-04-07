#include <UnityAsset/SerializedAsset/SerializedAssetFile.h>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    SerializedAssetFile::SerializedAssetFile() = default;

    SerializedAssetFile::~SerializedAssetFile() = default;

    SerializedAssetFile::SerializedAssetFile(SerializedAssetFile &&other) noexcept = default;

    SerializedAssetFile &SerializedAssetFile::operator =(SerializedAssetFile &&other) noexcept = default;

    SerializedAssetFile::SerializedAssetFile(Stream &&stream) : SerializedAssetFile() {
        stream.setByteOrder(Stream::ByteOrder::MostSignificantFirst);

        uint32_t metadataSize, fileSize;

        stream >> metadataSize >> fileSize;

        if(fileSize != stream.length())
            throw std::runtime_error("SerializedAssetFile: file size in the header is inconsistent with the stream length");

        uint32_t version;
        stream >> version;
        if(version != AssetVersion)
            throw std::runtime_error("SerializedAssetFile: unexpected file version");

        uint32_t dataOffset, fileFlags;
        stream >> dataOffset >> fileFlags;

        if(fileFlags != 0)
            throw std::runtime_error("SerializedAssetFile: unexpected file flags");

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
            m_Objects.emplace_back(metadataStream, dataArea);
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
}
