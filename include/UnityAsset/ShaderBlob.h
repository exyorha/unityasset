#ifndef UNITY_ASSET_SHADER_BLOB_H
#define UNITY_ASSET_SHADER_BLOB_H

#include <vector>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    class ShaderBlob {
    public:
        explicit ShaderBlob(
            const std::vector<uint32_t> &compressedLengths, const std::vector<uint32_t> &decompressedLengths, const std::vector<uint32_t> &offsets,
            const std::vector<unsigned char> &compressedData);

        ~ShaderBlob();

        ShaderBlob(const ShaderBlob &other) = delete;
        ShaderBlob &operator =(const ShaderBlob &other) = delete;

        ShaderBlob(ShaderBlob &&other) noexcept;
        ShaderBlob &operator =(ShaderBlob &&other) noexcept;


        void serialize(
            std::vector<uint32_t> &compressedLengths,
            std::vector<uint32_t> &decompressedLengths,
            std::vector<uint32_t> &offsets,
            std::vector<unsigned char> &compressedBlob) const;

        std::vector<Stream> entries;
    };

}

#endif
