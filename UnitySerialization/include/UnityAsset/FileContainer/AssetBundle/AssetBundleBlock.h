#ifndef UNITY_ASSET_FILE_CONTAINER_ASSET_BUNDLE_ASSET_BUNDLE_BLOCK_H
#define UNITY_ASSET_FILE_CONTAINER_ASSET_BUNDLE_ASSET_BUNDLE_BLOCK_H

#include <cstdint>

namespace UnityAsset {

    class Stream;

    struct AssetBundleBlock {
        uint32_t uncompressedSize;
        uint32_t compressedSize;
        uint16_t flags;
    };

    Stream &operator <<(Stream &stream, const AssetBundleBlock &block);
    Stream &operator >>(Stream &stream, AssetBundleBlock &block);
}

#endif
