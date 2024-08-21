#ifndef UNITY_ASSET_FILE_CONTAINER_ASSET_BUNDLE_ASSET_BUNDLE_DIRECTORY_H
#define UNITY_ASSET_FILE_CONTAINER_ASSET_BUNDLE_ASSET_BUNDLE_DIRECTORY_H

#include <array>
#include <vector>

#include <UnityAsset/FileContainer/AssetBundle/AssetBundleBlock.h>
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleDirectoryEntry.h>

namespace UnityAsset {

    class Stream;

    class AssetBundleDirectory {
    public:
        AssetBundleDirectory();
        ~AssetBundleDirectory();

        AssetBundleDirectory(const AssetBundleDirectory &other) = delete;
        AssetBundleDirectory &operator =(const AssetBundleDirectory &other) = delete;

        AssetBundleDirectory(AssetBundleDirectory &&other) noexcept;
        AssetBundleDirectory &operator =(AssetBundleDirectory &&other) noexcept;

        /*
         * Deserializing constructor.
         */
        explicit AssetBundleDirectory(Stream &&input);

        void serialize(Stream &output) const;

        std::array<unsigned char, 16> uncompressedDataHash;
        std::vector<AssetBundleBlock> blocks;
        std::vector<AssetBundleDirectoryEntry> files;
    };
}

#endif

