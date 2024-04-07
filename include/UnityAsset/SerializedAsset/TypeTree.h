#ifndef UNITY_ASSET_SERIALIZED_ASSET_TYPE_TREE_H
#define UNITY_ASSET_SERIALIZED_ASSET_TYPE_TREE_H

namespace UnityAsset {

    class Stream;

    class TypeTree {
    public:
        explicit TypeTree(Stream &stream);
        ~TypeTree();

        TypeTree(const TypeTree &other) = delete;
        TypeTree &operator =(const TypeTree &other) = delete;

        TypeTree(TypeTree &&other) noexcept;
        TypeTree &operator =(TypeTree &&other) noexcept;

        void serialize(Stream &stream) const;
    };

}

#endif
