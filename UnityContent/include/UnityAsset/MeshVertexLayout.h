#ifndef UNITY_ASSET_MESH_VERTEX_LAYOUT_H
#define UNITY_ASSET_MESH_VERTEX_LAYOUT_H

#include <vector>
#include <cstddef>

namespace UnityAsset {

    namespace UnityTypes {
        struct VertexData;
        struct ChannelInfo;
    }

    class MeshVertexLayout {
    public:
        explicit MeshVertexLayout(const UnityAsset::UnityTypes::VertexData &data);
        ~MeshVertexLayout();

        MeshVertexLayout(const MeshVertexLayout &other);
        MeshVertexLayout &operator =(const MeshVertexLayout &other);

        MeshVertexLayout(MeshVertexLayout &&other) noexcept;
        MeshVertexLayout &operator =(MeshVertexLayout &&other) noexcept;

        std::vector<size_t> streamPitches;
        std::vector<size_t> streamOffsets;
    };

    bool channelInUse(const UnityAsset::UnityTypes::ChannelInfo &channel);
    size_t channelAttributeSize(const UnityAsset::UnityTypes::ChannelInfo &channel);
    size_t channelElementSize(const UnityAsset::UnityTypes::ChannelInfo &channel);

    template<typename DestinationType, typename SourceType>
    std::vector<DestinationType> fetchAndConvertVertexStream(
        const unsigned char *vertexDataPointer,
        size_t stride,
        size_t dimension,
        size_t vertexCount,
        size_t paddingValues = 0) {

        std::vector<DestinationType> result;
        result.reserve(vertexCount * dimension);

        auto vertexPtr = vertexDataPointer;
        for(size_t index = 0; index < vertexCount; index++) {

            auto elements = reinterpret_cast<const SourceType *>(vertexPtr);

            for(size_t element = 0; element < dimension; element++) {
                result.emplace_back(*elements++);

                for(size_t paddingIndex = 0; paddingIndex < paddingValues; paddingIndex++) {
                    result.emplace_back(0);
                }
            }

            vertexPtr += stride;
        }

        return result;
    }

    std::vector<float> unpackVertexArray(
        const UnityAsset::UnityTypes::VertexData &data,
        const MeshVertexLayout &layout,
        const UnityAsset::UnityTypes::ChannelInfo &channel);
}

#endif
