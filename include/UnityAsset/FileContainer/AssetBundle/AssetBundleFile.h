#ifndef UNITY_ASSET_FILE_CONTAINER_ASSET_BUNDLE_ASSET_BUNDLE_FILE_H
#define UNITY_ASSET_FILE_CONTAINER_ASSET_BUNDLE_ASSET_BUNDLE_FILE_H

#include <UnityAsset/UnityCompression.h>
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleEntry.h>

#include <string_view>
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace UnityAsset {

    class Stream;

    class AssetBundleFile {
    public:
        AssetBundleFile();
        ~AssetBundleFile();

        AssetBundleFile(const AssetBundleFile &other) = delete;
        AssetBundleFile &operator =(const AssetBundleFile &other) = delete;

        AssetBundleFile(AssetBundleFile &&other) noexcept;
        AssetBundleFile &operator =(AssetBundleFile &&other) noexcept;


        /*
         * Deserializing constructor.
         */
        explicit AssetBundleFile(Stream &&input);


        void serialize(Stream &output) const;

        std::string unityVersion;
        std::string unityRevision;

        UnityCompressionType directoryCompression;
        UnityCompressionType dataCompression;
        size_t blockSize;
        std::optional<uint32_t> assetBundleCRC;

        std::vector<AssetBundleEntry> entries;

    private:
        static constexpr std::string_view AssetBundleSignature = "UnityFS";
        static constexpr uint32_t AssetBundleVersion = UINT32_C(6);

        using DirectoryFlags = uint32_t;

        static inline UnityCompressionType directoryCompressionType(DirectoryFlags flags) {
            return static_cast<UnityCompressionType>(flags & UINT32_C(0x3f));
        }

        static inline uint32_t directoryCompressionOptions(DirectoryFlags flags) {
            return flags & ~UINT32_C(0x3f);
        }

        static constexpr DirectoryFlags BlocksAndDirectoryInfoCombined = UINT32_C(0x40);
        static constexpr DirectoryFlags BlocksInfoAtTheEnd = UINT32_C(0x80);
        static constexpr DirectoryFlags OldWebPluginCompatibility = UINT32_C(0x100);
        static constexpr DirectoryFlags BlockInfoNeedPaddingAtStart = UINT32_C(0x200);
    };
}

#endif
