#include <UnityAsset/SerializedAsset/SerializedObject.h>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    SerializedObject::SerializedObject(Stream &input, Stream &objectDataArea) {

        input.alignPosition(4);
        input >> m_PathID;

        uint32_t byteStart, byteSize;
        input >> byteStart >> byteSize >> typeIndex;

        objectData = objectDataArea.createView(byteStart, byteSize);
    }

    SerializedObject::~SerializedObject() = default;

    SerializedObject::SerializedObject(SerializedObject &&other) noexcept = default;

    SerializedObject &SerializedObject::operator =(SerializedObject &&other) noexcept = default;

}

