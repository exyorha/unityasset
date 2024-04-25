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
            if(entry.filename().ends_with(".resource") || entry.filename().ends_with(".resS"))
                continue;

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

        return bundleObject;
    }

    LoadedSerializedAsset *LinkedEnvironment::addAsset(const std::string_view &name, const UnityAsset::Stream &stream) {
        return m_assets.emplace_back(std::make_unique<LoadedSerializedAsset>(name, stream)).get();
    }

    void LinkedEnvironment::link() {
        for(auto &asset: m_assets) {
            asset->link();
        }
    }
}
