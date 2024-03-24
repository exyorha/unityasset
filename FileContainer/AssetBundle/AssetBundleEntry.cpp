#include <UnityAsset/FileContainer/AssetBundle/AssetBundleEntry.h>

namespace UnityAsset {
    AssetBundleEntry::AssetBundleEntry(std::string &&filename, Stream &&data, uint32_t flags) : m_filename(std::move(filename)), m_data(std::move(data)), m_flags(flags) {

    }

    AssetBundleEntry::~AssetBundleEntry() = default;

    AssetBundleEntry::AssetBundleEntry(AssetBundleEntry &&other) noexcept = default;

    AssetBundleEntry &AssetBundleEntry::operator =(AssetBundleEntry &&other) noexcept = default;
}

