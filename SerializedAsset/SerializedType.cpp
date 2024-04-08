#include <UnityAsset/SerializedAsset/SerializedType.h>
#include <UnityAsset/Streams/Stream.h>

#include <cstdio>

namespace UnityAsset {

    SerializedType::SerializedType(Stream &input, bool isReferenceType, bool hasTypeTree) {
        input
            >> classID
            >> m_IsStrippedType
            >> m_ScriptTypeIndex;

        if((isReferenceType && m_ScriptTypeIndex >= 0) || classID == 114 /* MonoBehavior */) {
            input >> m_ScriptID.emplace();
        }

        input >> m_OldTypeHash;

        if(m_IsStrippedType)
            throw std::logic_error("unsupported m_IsStrippedType flag");

        if(hasTypeTree) {
            m_Type.emplace(input, isReferenceType);
        }
    }

    SerializedType::~SerializedType() = default;

    SerializedType::SerializedType(SerializedType &&other) noexcept = default;

    SerializedType &SerializedType::operator =(SerializedType &&other) noexcept = default;

    void SerializedType::serialize(Stream &output) const {
        output << classID << m_IsStrippedType << m_ScriptTypeIndex;

        if(m_ScriptID.has_value()) {
            output << *m_ScriptID;
        }

        output << m_OldTypeHash;

        if(m_Type.has_value()) {
            m_Type->serialize(output);
        }
    }

}
