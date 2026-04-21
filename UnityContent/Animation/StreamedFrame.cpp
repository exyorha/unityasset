#include <UnityAsset/Animation/StreamedFrame.h>
#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {


    StreamedFrame::StreamedFrame(Stream &stream) {
        stream >> m_time;

        uint32_t numberOfCurves;
        stream >> numberOfCurves;

        m_curves.reserve(numberOfCurves);

        for(uint32_t index = 0; index < numberOfCurves; index++) {
            m_curves.emplace_back(stream);
        }
    }

    StreamedFrame::~StreamedFrame() = default;

    StreamedFrame::StreamedFrame(const StreamedFrame &other) = default;

    StreamedFrame &StreamedFrame::operator =(const StreamedFrame &other) = default;

    StreamedFrame::StreamedFrame(StreamedFrame &&other) noexcept = default;

    StreamedFrame &StreamedFrame::operator =(StreamedFrame &&other) noexcept = default;

}
