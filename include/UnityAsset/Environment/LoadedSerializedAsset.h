#ifndef UNITY_ASSET_ENVIRONMENT_LOADED_SERIALIZED_ASSET_H
#define UNITY_ASSET_ENVIRONMENT_LOADED_SERIALIZED_ASSET_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace UnityAsset {

    class Stream;
    class Downcastable;
    class LinkedEnvironment;

    class LoadedSerializedAsset {
    public:
        LoadedSerializedAsset(const std::string_view &name, const Stream &dataStream);
        ~LoadedSerializedAsset();

        LoadedSerializedAsset(const LoadedSerializedAsset &other) = delete;
        LoadedSerializedAsset &operator =(const LoadedSerializedAsset &other) = delete;

        inline const std::string &name() const {
            return m_name;
        }

        inline const std::unordered_map<int64_t, std::unique_ptr<Downcastable>> &objects() const {
            return m_objects;
        }

        void link(const LinkedEnvironment *environment);

        Downcastable *resolvePathID(int64_t pathID) const;

        Downcastable *resolvePointer(int32_t fileID, int64_t pathID) const;

    private:
        struct AssetExternal {
            std::string pathName;
            LoadedSerializedAsset *asset;
        };

        std::string m_name;
        std::vector<AssetExternal> m_externals;
        std::unordered_map<int64_t, std::unique_ptr<Downcastable>> m_objects;
    };

}

#endif
