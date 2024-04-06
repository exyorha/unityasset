#include <UnityAsset/SerializedAsset/SerializedObject.h>

#include <UnityAsset/Streams/Stream.h>

#include <cinttypes>

namespace UnityAsset {

    SerializedObject::SerializedObject(Stream &input) {

        input.alignPosition(4); // This is what AssetStudio does. TODO: review if it *should* be 4, and not 8
        input >> m_PathID;

        input >> byteStart >> byteSize >> typeIndex;

        printf("path ID: %" PRId64 ", starts at: %u, size: %u bytes, type index: %u\n", m_PathID, byteStart, byteSize, typeIndex);

    }

    SerializedObject::~SerializedObject() = default;

    SerializedObject::SerializedObject(SerializedObject &&other) noexcept = default;

    SerializedObject &SerializedObject::operator =(SerializedObject &&other) noexcept = default;

}

