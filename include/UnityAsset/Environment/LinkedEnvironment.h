#ifndef UNITY_ASSET_ENVIRONMENT_LINKED_ENVIRONMENT_H
#define UNITY_ASSET_ENVIRONMENT_LINKED_ENVIRONMENT_H

#include <memory>
#include <vector>

namespace UnityAsset {

    class AssetBundleFile;
    class Stream;
    class LoadedSerializedAsset;

    namespace UnityClasses {
        struct AssetBundle;
    }

    class LinkedEnvironment {
    public:
        LinkedEnvironment();
        ~LinkedEnvironment();

        LinkedEnvironment(const LinkedEnvironment &other) = delete;
        LinkedEnvironment &operator =(const LinkedEnvironment &other) = delete;

        const UnityClasses::AssetBundle *addAssetBundle(const UnityAsset::AssetBundleFile &bundle);
        LoadedSerializedAsset *addAsset(const std::string_view &name, const UnityAsset::Stream &stream);

        inline const std::vector<std::unique_ptr<LoadedSerializedAsset>> &assets() const {
            return m_assets;
        }

        void link();

        LoadedSerializedAsset *resolveExternal(const std::string_view &assetName) const;

    private:
        static std::string_view getAssetBasename(const std::string_view &assetName);

        std::vector<std::unique_ptr<LoadedSerializedAsset>> m_assets;
    };
}

#endif
