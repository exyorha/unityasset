#ifndef UNITY_ASSET_SERIALIZED_ASSET_SERIALIZED_OBJECT_H
#define UNITY_ASSET_SERIALIZED_ASSET_SERIALIZED_OBJECT_H

#include <cstdint>

namespace UnityAsset {

    class Stream;

    class SerializedObject {
    public:
        explicit SerializedObject(Stream &input);
        ~SerializedObject();

        SerializedObject(const SerializedObject &other) = delete;
        SerializedObject &operator =(const SerializedObject &other) = delete;

        SerializedObject(SerializedObject &&other) noexcept;
        SerializedObject &operator =(SerializedObject &&other) noexcept;

        int64_t m_PathID;
        uint32_t byteStart;
        uint32_t byteSize;
        uint32_t typeIndex;
    };

}

#endif

