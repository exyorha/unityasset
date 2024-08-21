#ifndef UNITY_ASSET_SERIALIZED_ASSET_TYPE_TREE_H
#define UNITY_ASSET_SERIALIZED_ASSET_TYPE_TREE_H

#include <vector>
#include <optional>

#include <UnityAsset/SerializedAsset/TypeTreeNode.h>
#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    class Stream;

    class TypeTree {
    public:
        explicit TypeTree(Stream &stream, bool isRefType);
        ~TypeTree();

        TypeTree(const TypeTree &other) = delete;
        TypeTree &operator =(const TypeTree &other) = delete;

        TypeTree(TypeTree &&other) noexcept;
        TypeTree &operator =(TypeTree &&other) noexcept;

        struct ReferenceTypeData {
            std::string m_KlassName;
            std::string m_NameSpace;
            std::string m_AsmName;
        };

        void serialize(Stream &stream) const;

        std::vector<TypeTreeNode> m_Nodes;
        Stream m_StringBuffer;
        std::optional<ReferenceTypeData> referenceTypeData;
        std::optional<std::vector<int32_t>> m_TypeDependencies;
    };

}

#endif
