#include <UnityAsset/SerializedAsset/ExternalAssetData.h>

namespace UnityAsset {

    ExternalAssetData::ExternalAssetData() = default;

    ExternalAssetData::~ExternalAssetData() = default;

    ExternalAssetData::ExternalAssetData(const ExternalAssetData &other) = default;

    ExternalAssetData &ExternalAssetData::operator =(const ExternalAssetData &other) = default;

    ExternalAssetData::ExternalAssetData(ExternalAssetData &&other) noexcept = default;

    ExternalAssetData &ExternalAssetData::operator =(ExternalAssetData &&other) noexcept = default;
}
