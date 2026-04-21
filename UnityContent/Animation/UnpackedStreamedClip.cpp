#include <UnityAsset/Animation/UnpackedStreamedClip.h>
#include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>
#include <UnityAsset/Streams/Stream.h>

#include <cstring>

namespace UnityAsset {

    UnpackedStreamedClip::UnpackedStreamedClip(std::vector<unsigned char> &&data) {
        Stream stream(std::make_shared<InMemoryStreamBackingBuffer>(std::move(data)));
        stream.setByteOrder(Stream::ByteOrder::LeastSignificantFirst);

        while(stream.position() < stream.length()) {
            m_frames.emplace_back(stream);
        }

        m_frames.shrink_to_fit();

    }

    UnpackedStreamedClip::UnpackedStreamedClip(const std::vector<uint32_t> &data) : UnpackedStreamedClip(repack32(data)) {

    }

    UnpackedStreamedClip::~UnpackedStreamedClip() = default;

    UnpackedStreamedClip::UnpackedStreamedClip(const UnpackedStreamedClip &other) = default;

    UnpackedStreamedClip &UnpackedStreamedClip::operator =(const UnpackedStreamedClip &other) = default;

    UnpackedStreamedClip::UnpackedStreamedClip(UnpackedStreamedClip &&other) noexcept = default;

    UnpackedStreamedClip &UnpackedStreamedClip::operator =(UnpackedStreamedClip &&other) noexcept = default;

    std::vector<unsigned char> UnpackedStreamedClip::repack32(const std::vector<uint32_t> &data) {
        std::vector<unsigned char> output;

        output.resize(data.size() * sizeof(uint32_t));

        memcpy(output.data(), data.data(), data.size() * sizeof(uint32_t));

        return output;
    }

}
