#include <UnityAsset/SerializedAsset/SerializedObject.h>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    SerializedObject::SerializedObject() : m_PathID(0), typeIndex(0) {

    }

    SerializedObject::SerializedObject(Stream &input, Stream &objectDataArea, bool offset64) {

        input.alignPosition(4);
        input >> m_PathID;

        uint64_t byteStart;
        uint32_t byteSize;

        if(offset64) {
            input >> byteStart;
        } else {
            uint32_t byteStartNarrow;
            input >> byteStartNarrow;
            byteStart = byteStartNarrow;
        }

        input >> byteSize >> typeIndex;

        objectData = objectDataArea.createView(byteStart, byteSize);
    }

    SerializedObject::~SerializedObject() = default;

    SerializedObject::SerializedObject(SerializedObject &&other) noexcept = default;

    SerializedObject &SerializedObject::operator =(SerializedObject &&other) noexcept = default;

}

