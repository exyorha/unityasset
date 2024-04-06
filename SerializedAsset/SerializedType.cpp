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

        printf("class ID: %d, stripped: %d, script type index: %d, has script ID: %d\n", classID, m_IsStrippedType, m_ScriptTypeIndex, m_ScriptID.has_value());

        if(hasTypeTree) {
            m_Type.emplace(input);
        }
    }

    SerializedType::~SerializedType() = default;

    SerializedType::SerializedType(SerializedType &&other) noexcept = default;

    SerializedType &SerializedType::operator =(SerializedType &&other) noexcept = default;

}
