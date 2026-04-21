#ifndef UNITY_ASSET_ANIMATION_UNPACKED_STREAMED_CLIP_H
#define UNITY_ASSET_ANIMATION_UNPACKED_STREAMED_CLIP_H

#include <cstdint>
#include <vector>

#include <UnityAsset/Animation/StreamedFrame.h>

namespace UnityAsset {
    class UnpackedStreamedClip {
    public:
        explicit UnpackedStreamedClip(std::vector<unsigned char> &&data);
        explicit UnpackedStreamedClip(const std::vector<uint32_t> &data);
        ~UnpackedStreamedClip();

        UnpackedStreamedClip(const UnpackedStreamedClip &other);
        UnpackedStreamedClip &operator =(const UnpackedStreamedClip &other);

        UnpackedStreamedClip(UnpackedStreamedClip &&other) noexcept;
        UnpackedStreamedClip &operator =(UnpackedStreamedClip &&other) noexcept;

        inline const std::vector<StreamedFrame> &frames() const {
            return m_frames;
        }

    private:
        static std::vector<unsigned char> repack32(const std::vector<uint32_t> &data);

        std::vector<StreamedFrame> m_frames;
    };
}

#endif
