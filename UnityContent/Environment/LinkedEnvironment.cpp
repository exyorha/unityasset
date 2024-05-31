#include <UnityAsset/Environment/LinkedEnvironment.h>
#include <UnityAsset/Environment/LoadedSerializedAsset.h>

#include <UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h>

#include <UnityAsset/UnityTypes.h>

namespace UnityAsset {


    LinkedEnvironment::LinkedEnvironment() = default;

    LinkedEnvironment::~LinkedEnvironment() = default;

    const UnityClasses::AssetBundle *LinkedEnvironment::addAssetBundle(const UnityAsset::AssetBundleFile &bundle) {
        const UnityClasses::AssetBundle *bundleObject = nullptr;

        for(const auto &entry: bundle.entries) {
            if(entry.filename().ends_with(".resource") || entry.filename().ends_with(".resS")) {
                m_resourceFiles.emplace(getAssetBasename(entry.filename()), entry.data());
            } else {

                auto asset = addAsset(entry.filename(), entry.data());
                if(!bundleObject) {
                    for(const auto &object: asset->objects()) {
                        bundleObject = object_cast<UnityClasses::AssetBundle>(object.second.get());
                        if(bundleObject) {
                            break;
                        }
                    }
                }
            }
        }

        return bundleObject;
    }

    std::optional<Stream> LinkedEnvironment::resolveStreamedDataFile(const std::string_view &fileName) const {
        auto it = m_resourceFiles.find(std::string(getAssetBasename(fileName)));
        if(it == m_resourceFiles.end()) {
            return std::nullopt;
        }

        return it->second;
    }

    LoadedSerializedAsset *LinkedEnvironment::addAsset(const std::string_view &name, const UnityAsset::Stream &stream) {
        return m_assets.emplace_back(std::make_unique<LoadedSerializedAsset>(name, stream)).get();
    }

    void LinkedEnvironment::link() {
        for(auto &asset: m_assets) {
            asset->link(this);
        }
    }

    std::string_view LinkedEnvironment::getAssetBasename(const std::string_view &assetName) {
        auto pos = assetName.find_last_of(":/");
        if(pos == std::string_view::npos) {
            return assetName;
        } else {
            return assetName.substr(pos + 1);
        }
    }

    LoadedSerializedAsset *LinkedEnvironment::resolveExternal(const std::string_view &assetName) const {
        std::string_view basename = getAssetBasename(assetName);

        for(const auto &asset: m_assets) {
            if(getAssetBasename(asset->name()) == basename) {
                return asset.get();
            }
        }

        return nullptr;
    }

}
