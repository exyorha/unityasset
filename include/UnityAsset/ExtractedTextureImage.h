#ifndef UNITY_ASSET_EXTRACTED_TEXTURE_IMAGE_H
#define UNITY_ASSET_EXTRACTED_TEXTURE_IMAGE_H

#include <vector>
#include <cstdint>

#include <UnityAsset/UnityTextureTypes.h>

namespace UnityAsset {

    class ExtractedTextureImage {
    public:
        ExtractedTextureImage(
            const unsigned char *textureData,
            const TextureFormatClassification &format,
            const TextureSubImage &image);

        ~ExtractedTextureImage();

        ExtractedTextureImage(const ExtractedTextureImage &other);
        ExtractedTextureImage &operator =(const ExtractedTextureImage &other);

        ExtractedTextureImage(ExtractedTextureImage &&other) noexcept;
        ExtractedTextureImage &operator =(ExtractedTextureImage &&other) noexcept;

        inline const std::vector<uint32_t> &imageData() const {
            return m_imageData;
        }

        inline std::vector<uint32_t> &&imageData() {
            return std::move(m_imageData);
        }

        inline const TextureFormatClassification &format() const {
            return TextureFormatClassification::RGBA8;
        }

        inline const ImageStorageInfo &storageInfo() const {
            return m_storageInfo;
        }

        std::vector<unsigned char> compressToPNG() const;

        static int getPNGCompressionLevel();
        static void setPNGCompressionLevel(int level);

    private:
        void decompressViaBCDecWorker(const unsigned char *textureData, const UnityAsset::TextureSubImage &image,
                                      void (*worker)(const void* compressedBlock, void* decompressedBlock, int destinationPitch),
                                      unsigned int blockBytes);

        std::vector<uint32_t> m_imageData;
        ImageStorageInfo m_storageInfo;
    };

}

#endif
