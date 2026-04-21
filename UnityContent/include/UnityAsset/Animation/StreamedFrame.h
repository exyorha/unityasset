#ifndef UNITY_ASSET_ANIMATION_STREAMED_FRAME_H
#define UNITY_ASSET_ANIMATION_STREAMED_FRAME_H

#include <UnityAsset/Animation/StreamedCurveKey.h>

#include <vector>

namespace UnityAsset {

    class Stream;

    class StreamedFrame {
    public:
        explicit StreamedFrame(Stream &stream);
        ~StreamedFrame();

        StreamedFrame(const StreamedFrame &other);
        StreamedFrame &operator =(const StreamedFrame &other);

        StreamedFrame(StreamedFrame &&other) noexcept;
        StreamedFrame &operator =(StreamedFrame &&other) noexcept;

        inline float time() const {
            return m_time;
        }

        inline const std::vector<StreamedCurveKey> &curves() const {
            return m_curves;
        }

    private:
        float m_time;
        std::vector<StreamedCurveKey> m_curves;
    };
}

#endif
