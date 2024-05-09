#ifndef UNITYASSET_ENVIRONMENT_EXTERNAL_ASSET_DATA_H
#define UNITYASSET_ENVIRONMENT_EXTERNAL_ASSET_DATA_H

#include <optional>
#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    class ExternalAssetData {
    protected:
        ExternalAssetData();
        ~ExternalAssetData();

        ExternalAssetData(const ExternalAssetData &other);
        ExternalAssetData &operator =(const ExternalAssetData &other);

        ExternalAssetData(ExternalAssetData &&other) noexcept;
        ExternalAssetData &operator =(ExternalAssetData &&other) noexcept;

    public:
        inline void link(std::optional<Stream> &&stream) {
            m_stream = std::move(stream);
        }

        inline bool hasStream() const {
            return m_stream.has_value();
        }

        inline operator bool() const {
            return m_stream.has_value();
        }

        inline const Stream &stream() const {
            return m_stream.value();
        }

    private:
        std::optional<Stream> m_stream;
    };

}

#endif
