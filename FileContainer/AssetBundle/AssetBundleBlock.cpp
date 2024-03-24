#include <UnityAsset/FileContainer/AssetBundle/AssetBundleBlock.h>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    Stream &operator <<(Stream &stream, const AssetBundleBlock &block) {
        return stream << block.uncompressedSize << block.compressedSize << block.flags;
    }

    Stream &operator >>(Stream &stream, AssetBundleBlock &block) {
        return stream >> block.uncompressedSize >> block.compressedSize >> block.flags;
    }
}

