#ifndef UNITY_ASSET_SERIALIZED_ASSET_SERIALIZED_ASSET_FILE_H
#define UNITY_ASSET_SERIALIZED_ASSET_SERIALIZED_ASSET_FILE_H

#include <cstdint>
#include <string>
#include <vector>

#include <UnityAsset/SerializedAsset/SerializedType.h>
#include <UnityAsset/SerializedAsset/SerializedObject.h>

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

        std::string unityVersion;
        uint32_t platform = 0;
        bool typeTreeEnabled = false;
        std::vector<SerializedType> m_Types;
        std::vector<SerializedObject> m_Objects;
        //std::vector<LocalSerializedObjectIdentifier> m_ScriptTypes;

    private:
        static constexpr uint32_t AssetVersion = 21;
    };
}

#endif
