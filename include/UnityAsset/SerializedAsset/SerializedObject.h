#ifndef UNITY_ASSET_SERIALIZED_ASSET_SERIALIZED_OBJECT_H
#define UNITY_ASSET_SERIALIZED_ASSET_SERIALIZED_OBJECT_H

#include <cstdint>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    class Stream;

    class SerializedObject {
    public:
        explicit SerializedObject(Stream &input, Stream &objectDataArea);
        ~SerializedObject();

        SerializedObject(const SerializedObject &other) = delete;
        SerializedObject &operator =(const SerializedObject &other) = delete;

        SerializedObject(SerializedObject &&other) noexcept;
        SerializedObject &operator =(SerializedObject &&other) noexcept;

        int64_t m_PathID;
        Stream objectData;
        uint32_t typeIndex;
    };

}

#endif

