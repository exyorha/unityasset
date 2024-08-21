#ifndef UNITY_ASSET_SERIALIZED_ASSET_SERIALIZED_ASSET_FILE_H
#define UNITY_ASSET_SERIALIZED_ASSET_SERIALIZED_ASSET_FILE_H

#include <cstdint>
#include <string>
#include <vector>

#include <UnityAsset/SerializedAsset/SerializedType.h>
#include <UnityAsset/SerializedAsset/SerializedObject.h>
#include <UnityAsset/SerializedAsset/LocalSerializedObjectIdentifier.h>
#include <UnityAsset/SerializedAsset/FileIdentifier.h>

namespace UnityAsset {

    class Stream;

    class SerializedAssetFile {
    public:
        SerializedAssetFile();
        ~SerializedAssetFile();

        SerializedAssetFile(const SerializedAssetFile &other) = delete;
        SerializedAssetFile &operator =(const SerializedAssetFile &other) = delete;

        SerializedAssetFile(SerializedAssetFile &&other) noexcept;
        SerializedAssetFile &operator =(SerializedAssetFile &&other) noexcept;


        /*
         * Deserializing constructor.
         */
        explicit SerializedAssetFile(Stream &&input);

        void serialize(Stream &output) const;

        uint32_t assetVersion;
        std::string unityVersion;
        int32_t platform = 0;
        bool typeTreeEnabled = false;
        std::vector<SerializedType> m_Types;
        std::vector<SerializedObject> m_Objects;
        std::vector<LocalSerializedObjectIdentifier> m_ScriptTypes;
        std::vector<FileIdentifier> m_Externals;
        std::vector<SerializedType> m_RefTypes;
        std::string userInformation;
    };
}

#endif
