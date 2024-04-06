#ifndef UNITY_ASSET_SERIALIZED_ASSET_SERIALIZED_TYPE_H
#define UNITY_ASSET_SERIALIZED_ASSET_SERIALIZED_TYPE_H

#include <cstdint>
#include <array>
#include <optional>

#include <UnityAsset/SerializedAsset/TypeTree.h>

namespace UnityAsset {

    class Stream;

    using TypeHash = std::array<uint8_t, 16>;

    class SerializedType {
    public:
        SerializedType(Stream &input, bool isReferenceType, bool hasTypeTree);
        ~SerializedType();

        SerializedType(const SerializedType &other) = delete;
        SerializedType &operator =(const SerializedType &other) = delete;

        SerializedType(SerializedType &&other) noexcept;
        SerializedType &operator =(SerializedType &&other) noexcept;

        int32_t classID;
        bool m_IsStrippedType;
        int16_t m_ScriptTypeIndex;
        std::optional<TypeHash> m_ScriptID;
        TypeHash m_OldTypeHash;
        std::optional<TypeTree> m_Type;
    };

}

#endif
