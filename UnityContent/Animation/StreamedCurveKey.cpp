#include <UnityAsset/Animation/StreamedCurveKey.h>
#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {


    StreamedCurveKey::StreamedCurveKey(Stream &stream) {
        stream >> m_curveIndex;
        stream >> m_coefficients[0];
        stream >> m_coefficients[1];
        stream >> m_coefficients[2];
        stream >> m_coefficients[3];
    }

    StreamedCurveKey::~StreamedCurveKey() = default;

    StreamedCurveKey::StreamedCurveKey(const StreamedCurveKey &other) = default;

    StreamedCurveKey &StreamedCurveKey::operator =(const StreamedCurveKey &other) = default;

    StreamedCurveKey::StreamedCurveKey(StreamedCurveKey &&other) noexcept = default;

    StreamedCurveKey &StreamedCurveKey::operator =(StreamedCurveKey &&other) noexcept = default;

    float StreamedCurveKey::evaluate(float deltaTime) const {
        return deltaTime * (deltaTime * ((deltaTime * m_coefficients[0]) + m_coefficients[1]) + m_coefficients[2]) + m_coefficients[3];
    }
}
