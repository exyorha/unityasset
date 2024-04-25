#include <UnityAsset/Environment/LoadedSerializedAsset.h>
#include <UnityAsset/Environment/Downcastable.h>

#include <UnityAsset/SerializedAsset/SerializedAssetFile.h>

#include <UnityAsset/Streams/Stream.h>

#include <cinttypes>

namespace UnityAsset {

    LoadedSerializedAsset::LoadedSerializedAsset(const std::string_view &name, const Stream &dataStream) :
        m_name(name) {

        SerializedAssetFile file((Stream(dataStream)));

        for(const auto &external: file.m_Externals) {
            printf("external: %s, type %u\n", external.pathName.c_str(), external.type);
        }

        m_objects.reserve(file.m_Objects.size());

        for(const auto &object: file.m_Objects) {
            m_objects.emplace(object.m_PathID, Downcastable::loadObject(file.m_Types.at(object.typeIndex), object.objectData));
        }
    }

    LoadedSerializedAsset::~LoadedSerializedAsset() = default;

    void LoadedSerializedAsset::link() {
        for(const auto &object: m_objects) {
            if(object.second) {
                object.second->link(this);
            }
        }
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
            fprintf(stderr, "LoadedSerializedAsset::resolvePointer: need to resolve a pointer to a different asset: file ID %d, path ID %" PRId64 "\n",
                    fileID, pathID);
            return nullptr;
        }
    }
}
