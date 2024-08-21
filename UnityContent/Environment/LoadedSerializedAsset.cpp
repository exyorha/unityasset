#include <UnityAsset/Environment/LoadedSerializedAsset.h>
#include <UnityAsset/Environment/ObjectFactory.h>
#include <UnityAsset/Environment/LinkedEnvironment.h>

#include <UnityAsset/SerializedAsset/SerializedAssetFile.h>
#include <UnityAsset/SerializedAsset/Downcastable.h>

#include <UnityAsset/Streams/Stream.h>

#include <cinttypes>

namespace UnityAsset {

    LoadedSerializedAsset::LoadedSerializedAsset(const std::string_view &name, const Stream &dataStream) :
        m_name(name), m_linkingWithEnvironment(nullptr) {

        SerializedAssetFile file((Stream(dataStream)));

        m_externals.reserve(file.m_Externals.size());
        for(const auto &external: file.m_Externals) {
            auto &preparedExternal = m_externals.emplace_back();
            preparedExternal.pathName = external.pathName;
            preparedExternal.asset = nullptr;
        }

        m_objects.reserve(file.m_Objects.size());

        for(const auto &object: file.m_Objects) {
            m_objects.emplace(object.m_PathID, loadObject(file.m_Types.at(object.typeIndex), object.objectData));
        }
    }

    LoadedSerializedAsset::~LoadedSerializedAsset() = default;

    void LoadedSerializedAsset::link(const LinkedEnvironment *environment) {
        m_linkingWithEnvironment = environment;

        for(auto &external: m_externals) {
            external.asset = environment->resolveExternal(external.pathName);
        }

        for(const auto &object: m_objects) {
            if(object.second) {
                object.second->link(this);
            }
        }

        m_linkingWithEnvironment = nullptr;
    }

    Downcastable *LoadedSerializedAsset::resolvePathID(int64_t pathID) const {
        if(pathID == 0) {
            return nullptr;
        }

        auto it = m_objects.find(pathID);
        if(it == m_objects.end()) {
            fprintf(stderr, "LoadedSerializedAsset::resolvePathID: attempted to get the object with path ID %" PRId64 ", but no such object was defined\n",
                    pathID);

            return nullptr;
        }

        if(!it->second) {
            fprintf(stderr, "LoadedSerializedAsset::resolvePathID: attempted to get the object with path ID %" PRId64 ", but this object could not be deserialized\n",
                    pathID);
        }

        return it->second.get();
    }

    Downcastable *LoadedSerializedAsset::resolvePointer(int32_t fileID, int64_t pathID) const {
        if(fileID == 0) {
            return resolvePathID(pathID);
        } else {
            const auto &external = m_externals.at(fileID - 1);
            if(external.asset == nullptr) {

                fprintf(stderr, "LoadedSerializedAsset::resolvePointer: pointer to an external: file ID %d ('%s'), path ID %" PRId64 ": the corresponding asset was not found or wasn't loaded\n",
                        fileID, external.pathName.c_str(), pathID);
                return nullptr;
            }

            return external.asset->resolvePathID(pathID);
        }
    }

    std::optional<Stream> LoadedSerializedAsset::resolveStreamedDataFile(const std::string_view &fileName) const {
        return m_linkingWithEnvironment->resolveStreamedDataFile(fileName);
    }

}
