#ifndef UNITY_ASSET_SERIALIZED_ASSET_ASSET_LINKER_H
#define UNITY_ASSET_SERIALIZED_ASSET_ASSET_LINKER_H

#include <cstdint>
#include <optional>
#include <string_view>

namespace UnityAsset {

    class Downcastable;
    class Stream;

    class AssetLinker {
    protected:
        AssetLinker();
        ~AssetLinker();

        AssetLinker(const AssetLinker &other) = delete;
        AssetLinker &operator =(const AssetLinker &other) = delete;

    public:

        virtual Downcastable *resolvePointer(int32_t fileID, int64_t pathID) const = 0;
        virtual std::optional<Stream> resolveStreamedDataFile(const std::string_view &fileName) const = 0;

    };

}

#endif
