#include <UnityAsset/SerializedAsset/TypeTreeNode.h>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    TypeTreeNode::TypeTreeNode(Stream &stream) {
        stream
            >> m_Version
            >> m_Level
            >> m_TypeFlags
            >> m_TypeStrOffset
            >> m_NameStrOffset
            >> m_ByteSize
            >> m_Index
            >> m_MetaFlag
            >> m_RefTypeHash;
    }

    TypeTreeNode::~TypeTreeNode() = default;

    TypeTreeNode::TypeTreeNode(TypeTreeNode &&other) noexcept = default;

    TypeTreeNode &TypeTreeNode::operator =(TypeTreeNode &&other) noexcept = default;

    void TypeTreeNode::serialize(Stream &stream) const {
        stream
            << m_Version
            << m_Level
            << m_TypeFlags
            << m_TypeStrOffset
            << m_NameStrOffset
            << m_ByteSize
            << m_Index
            << m_MetaFlag
            << m_RefTypeHash;
    }
}

