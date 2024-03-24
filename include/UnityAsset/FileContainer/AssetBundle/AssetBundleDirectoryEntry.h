#ifndef UNITY_ASSET_FILE_CONTAINER_ASSET_BUNDLE_ASSET_BUNDLE_DIRECTORY_ENTRY_H
#define UNITY_ASSET_FILE_CONTAINER_ASSET_BUNDLE_ASSET_BUNDLE_DIRECTORY_ENTRY_H

#include <cstdint>
#include <string_view>

namespace UnityAsset {

    class Stream;

    struct AssetBundleDirectoryEntry {
        uint64_t fileOffset;
        uint64_t fileSize;
        uint32_t fileFlags;
        std::string_view path;
    };

    Stream &operator <<(Stream &stream, const AssetBundleDirectoryEntry &directoryEntry);
    Stream &operator >>(Stream &stream, AssetBundleDirectoryEntry &directoryEntry);
}

#endif

