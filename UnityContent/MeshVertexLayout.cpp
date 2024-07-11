#include <UnityAsset/MeshVertexLayout.h>
#include <UnityAsset/UnityTypes.h>
#include <UnityAsset/UnityEnums.h>
#include <UnityAsset/HalfFloat.h>

#include <stdexcept>

namespace UnityAsset {
    size_t channelAttributeSize(const UnityAsset::UnityTypes::ChannelInfo &channel) {
        return (channel.dimension & 15) * channelElementSize(channel);
    }

    bool channelInUse(const UnityAsset::UnityTypes::ChannelInfo &channel) {
        return (channel.dimension & 15) != 0;
    }

    size_t channelElementSize(const UnityAsset::UnityTypes::ChannelInfo &channel) {
        switch(static_cast<UnityAsset::VertexAttributeFormat>(channel.format)) {
            case UnityAsset::VertexAttributeFormat::Float32:
                return 4;

            case UnityAsset::VertexAttributeFormat::Float16:
                return 2;

            case UnityAsset::VertexAttributeFormat::UNorm8:
                return 1;

            case UnityAsset::VertexAttributeFormat::UInt32:
                return 4;

            default:
                throw std::runtime_error("unsupported channel format " + std::to_string(channel.format));
        }
    }

    MeshVertexLayout::MeshVertexLayout(const UnityAsset::UnityTypes::VertexData &data) {

        for(const auto &channel: data.m_Channels) {
            if(!channelInUse(channel))
                continue;

            if(streamPitches.size() <= channel.stream) {
                streamPitches.resize(channel.stream + 1, 0);
            }

            streamPitches[channel.stream] = std::max(streamPitches[channel.stream], channel.offset + channelAttributeSize(channel));
        }

        streamOffsets.reserve(streamPitches.size());
        size_t nextStreamOffset = 0;

        for(auto pitch: streamPitches) {
            if(pitch != 0) {
                nextStreamOffset = (nextStreamOffset + 15) & ~15;
            }

            streamOffsets.emplace_back(nextStreamOffset);
            nextStreamOffset += data.m_VertexCount * pitch;
        }

        if(nextStreamOffset < data.m_DataSize.size() || nextStreamOffset > ((data.m_DataSize.size() + 15) & ~15))
            throw std::runtime_error("mesh vertex layout is inconsistent with the channel layout: " + std::to_string(nextStreamOffset) + ", expected " + std::to_string(data.m_DataSize.size()));

    }

    MeshVertexLayout::MeshVertexLayout(const MeshVertexLayout &other) = default;

    MeshVertexLayout &MeshVertexLayout::operator =(const MeshVertexLayout &other) = default;

    MeshVertexLayout::MeshVertexLayout(MeshVertexLayout &&other) noexcept = default;

    MeshVertexLayout &MeshVertexLayout::operator =(MeshVertexLayout &&other) noexcept = default;

    MeshVertexLayout::~MeshVertexLayout() = default;

    std::vector<float> unpackVertexArray(
        const UnityAsset::UnityTypes::VertexData &data,
        const MeshVertexLayout &layout,
        const UnityAsset::UnityTypes::ChannelInfo &channel) {

        switch(channel.format) {
            case static_cast<int32_t>(UnityAsset::VertexAttributeFormat::Float32):
                return fetchAndConvertVertexStream<float, float>(
                    data.m_DataSize.data() + layout.streamOffsets.at(channel.stream) + channel.offset,
                    layout.streamPitches.at(channel.stream),
                    channel.dimension,
                    data.m_VertexCount);

            case static_cast<int32_t>(UnityAsset::VertexAttributeFormat::Float16):
                return fetchAndConvertVertexStream<float, HalfFloat>(
                    data.m_DataSize.data() + layout.streamOffsets.at(channel.stream) + channel.offset,
                    layout.streamPitches.at(channel.stream),
                    channel.dimension,
                    data.m_VertexCount);

            case static_cast<int32_t>(UnityAsset::VertexAttributeFormat::UInt32):
                return fetchAndConvertVertexStream<float, uint32_t>(
                    data.m_DataSize.data() + layout.streamOffsets.at(channel.stream) + channel.offset,
                    layout.streamPitches.at(channel.stream),
                    channel.dimension,
                    data.m_VertexCount);

            default:
                throw std::runtime_error("unsupported vertex data format: " + std::to_string(static_cast<unsigned int>(channel.format)));
        }
    }
}
