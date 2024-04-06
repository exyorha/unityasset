#include <UnityAsset/SerializedAsset/TypeTree.h>

#include <stdexcept>

namespace UnityAsset {

    TypeTree::TypeTree(Stream &stream) {
        (void)stream;
        throw std::logic_error("Type tree deserialization is not currently supported");
    }

    TypeTree::~TypeTree() = default;

    TypeTree::TypeTree(TypeTree &&other) noexcept = default;

    TypeTree &TypeTree::operator =(TypeTree &&other) noexcept = default;
}
