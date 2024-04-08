#ifndef UNITY_ASSET_TYPE_TREE_NODE_H
#define UNITY_ASSET_TYPE_TREE_NODE_H

#include <cstdint>

namespace UnityAsset {

    class Stream;

    class TypeTreeNode {
    public:
        explicit TypeTreeNode(Stream &stream);
        ~TypeTreeNode();

        TypeTreeNode(const TypeTreeNode &other) = delete;
        TypeTreeNode &operator =(const TypeTreeNode &other) = delete;

        TypeTreeNode(TypeTreeNode &&other) noexcept;
        TypeTreeNode &operator =(TypeTreeNode &&other) noexcept;

        uint16_t m_Version;
        uint8_t m_Level;
        uint8_t m_TypeFlags;
        uint32_t m_TypeStrOffset;
        uint32_t m_NameStrOffset;
        int32_t m_ByteSize;
        int32_t m_Index;
        int32_t m_MetaFlag;
        uint64_t m_RefTypeHash;

        void serialize(Stream &stream) const;
    };
}

#endif
