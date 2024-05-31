#include <UnityAsset/FileContainer/AssetBundle/AssetBundleDirectory.h>
#include <UnityAsset/Streams/Stream.h>

#include <cstring>

namespace UnityAsset {

    AssetBundleDirectory::AssetBundleDirectory() {
        memset(uncompressedDataHash.data(), 0, uncompressedDataHash.size());
    }

    AssetBundleDirectory::~AssetBundleDirectory() = default;

    AssetBundleDirectory::AssetBundleDirectory(AssetBundleDirectory &&other) noexcept = default;

    AssetBundleDirectory &AssetBundleDirectory::operator =(AssetBundleDirectory &&other) noexcept = default;

    AssetBundleDirectory::AssetBundleDirectory(Stream &&input) {
        input.setByteOrder(Stream::ByteOrder::MostSignificantFirst);

        input >> uncompressedDataHash;

        uint32_t blockCount;
        input >> blockCount;

        blocks.resize(blockCount);
        input >> blocks;

        uint32_t fileCount;
        input >> fileCount;

        files.resize(fileCount);
        input >> files;
    }


    void AssetBundleDirectory::serialize(Stream &output) const {
        output.setByteOrder(Stream::ByteOrder::MostSignificantFirst);

        output << uncompressedDataHash;
        output << static_cast<uint32_t>(blocks.size());
        output << blocks;
        output << static_cast<uint32_t>(files.size());
        output << files;
    }
}
