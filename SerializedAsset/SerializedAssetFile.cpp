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

        printf("header ended at %zu, metadata size: %u, data offset: %u, type tree enabled: %d\n", stream.position(), metadataSize, dataOffset, typeTreeEnabled);

        int32_t typeCount;
        metadataStream >> typeCount;

        m_Types.reserve(typeCount);

        printf("type count: %d\n", typeCount);

        for(int32_t element = 0; element < typeCount; element++) {
            m_Types.emplace_back(metadataStream, false, typeTreeEnabled);
        }

        int32_t objectCount;
        metadataStream >> objectCount;

        printf("object count: %d\n", objectCount);

        m_Objects.reserve(objectCount);

        for(int32_t element = 0; element < objectCount; element++) {
            m_Objects.emplace_back(metadataStream);
        }

        int32_t scriptTypeCount;
        metadataStream >> scriptTypeCount;

        printf("script type count: %d\n", scriptTypeCount);
/*
        m_ScriptTypes.reserve(scriptTypeCount);

        for(int32_t element = 0; element < scriptTypeCount; element++) {
            m_ScriptTypes.emplace_back(metadataStream);
        }
*/
        printf("metadata position: %zu out of %zu\n", metadataStream.position(), metadataStream.length());

    }
}
