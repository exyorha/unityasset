#ifndef UNITY_ASSET_ANIMATION_STREAMED_CURVE_KEY_H
#define UNITY_ASSET_ANIMATION_STREAMED_CURVE_KEY_H

#include <cstdint>
#include <array>

namespace UnityAsset {

    class Stream;

    class StreamedCurveKey {
    public:
        explicit StreamedCurveKey(Stream &stream);
        ~StreamedCurveKey();

        StreamedCurveKey(const StreamedCurveKey &other);
        StreamedCurveKey &operator =(const StreamedCurveKey &other);

        StreamedCurveKey(StreamedCurveKey &&other) noexcept;
        StreamedCurveKey &operator =(StreamedCurveKey &&other) noexcept;

        inline uint32_t curveIndex() const {
            return m_curveIndex;
        }

        float evaluate(float deltaTime) const;

    private:
        uint32_t m_curveIndex;
        std::array<float, 4> m_coefficients;
    };
}

#endif
