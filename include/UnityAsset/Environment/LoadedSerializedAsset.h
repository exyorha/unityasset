#ifndef UNITY_ASSET_ENVIRONMENT_LOADED_SERIALIZED_ASSET_H
#define UNITY_ASSET_ENVIRONMENT_LOADED_SERIALIZED_ASSET_H

#include <string>
#include <memory>
#include <unordered_map>

namespace UnityAsset {

    class Stream;
    class Downcastable;

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

        void link();

        Downcastable *resolvePathID(int64_t pathID) const;

        Downcastable *resolvePointer(int32_t fileID, int64_t pathID) const;

    private:
        std::string m_name;
        std::unordered_map<int64_t, std::unique_ptr<Downcastable>> m_objects;
    };

}

#endif
