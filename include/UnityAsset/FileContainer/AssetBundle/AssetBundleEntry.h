#ifndef UNITY_ASSET_FILE_CONTAINER_ASSET_BUNDLE_ASSET_BUNDLE_ENTRY_H
#define UNITY_ASSET_FILE_CONTAINER_ASSET_BUNDLE_ASSET_BUNDLE_ENTRY_H

#include <string>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    class AssetBundleEntry {
    public:
        AssetBundleEntry(std::string &&filename, Stream &&data, uint32_t flags);
        ~AssetBundleEntry();

        AssetBundleEntry(const AssetBundleEntry &other) = delete;
        AssetBundleEntry &operator =(const AssetBundleEntry &other) = delete;

        AssetBundleEntry(AssetBundleEntry &&other) noexcept;
        AssetBundleEntry &operator =(AssetBundleEntry &&other) noexcept;

        inline const std::string &filename() const {
            return m_filename;
        }

        inline const Stream &data() const {
            return m_data;
        }

        inline uint32_t flags() const {
            return m_flags;
        }

    private:
        std::string m_filename;
        Stream m_data;
        uint32_t m_flags;
    };
}

#endif
