#include <UnityAsset/ExtractedTextureImage.h>

#include <stdexcept>

#include "bcdec.h"
#include "stb_image_write.h"
#include "stb_image_write_config.h"

namespace UnityAsset {


    ExtractedTextureImage::ExtractedTextureImage(
        const unsigned char *textureData,
        const TextureFormatClassification &format,
        const TextureSubImage &image) :

        m_imageData(image.storageInfo().storedWidth() * image.storageInfo().activeHeight()),
        m_storageInfo(
            image.storageInfo().activeWidth(), image.storageInfo().activeHeight(),
            image.storageInfo().storedWidth(), image.storageInfo().storedHeight(),
            sizeof(uint32_t) * image.storageInfo().storedWidth() * image.storageInfo().activeHeight()) {

        switch(format.encodingClass()) {
            case UnityAsset::TextureEncodingClass::DXT1:
                decompressViaBCDecWorker(textureData, image, bcdec_bc1, BCDEC_BC1_BLOCK_SIZE);
                break;

            case UnityAsset::TextureEncodingClass::DXT5:
                decompressViaBCDecWorker(textureData, image, bcdec_bc3, BCDEC_BC3_BLOCK_SIZE);
                break;

            case UnityAsset::TextureEncodingClass::BC7:
                decompressViaBCDecWorker(textureData, image, bcdec_bc7, BCDEC_BC7_BLOCK_SIZE);
                break;

            default:
                throw std::runtime_error("ExtractedTextureImage: texture encoding class is not supported: " +
                    std::to_string(static_cast<unsigned int>(format.encodingClass())));
        }
    }

    ExtractedTextureImage::~ExtractedTextureImage() = default;

    ExtractedTextureImage::ExtractedTextureImage(const ExtractedTextureImage &other) = default;

    ExtractedTextureImage &ExtractedTextureImage::operator =(const ExtractedTextureImage &other) = default;

    ExtractedTextureImage::ExtractedTextureImage(ExtractedTextureImage &&other) noexcept = default;

    ExtractedTextureImage &ExtractedTextureImage::operator =(ExtractedTextureImage &&other) noexcept = default;

    void ExtractedTextureImage::decompressViaBCDecWorker(
        const unsigned char *textureData,
        const UnityAsset::TextureSubImage &image,
        void (*worker)(const void* compressedBlock, void* decompressedBlock, int destinationPitch),
        unsigned int blockBytes) {

        auto source = textureData + image.offset();
        auto destination = m_imageData.data();

        unsigned int rows = image.storageInfo().storedHeight() / 4;
        unsigned int columns = image.storageInfo().storedWidth() / 4;

        for(unsigned int row = 0; row < rows; row++) {
            for(unsigned int column = 0; column < columns; column++) {
                worker(source, destination, image.storageInfo().storedWidth() * sizeof(uint32_t));

                source += blockBytes;
                destination += 4;
            }

            destination += 3 * image.storageInfo().storedWidth();
        }
    }

    std::vector<unsigned char> ExtractedTextureImage::compressToPNG() const {
        if(m_imageData.size() * sizeof(uint32_t) != m_storageInfo.dataLength())
            throw std::logic_error("the image data storage doesn't have the expected length");

        auto writer = static_cast<stbi_write_func *>([](void *context, void *data, int size) {
            auto &output = *static_cast<std::vector<unsigned char> *>(context);

            output.insert(output.end(), static_cast<const unsigned char *>(data), static_cast<const unsigned char *>(data) + size);
        });

        std::vector<unsigned char> output;

        auto result = stbi_write_png_to_func(writer, &output, m_storageInfo.activeWidth(), m_storageInfo.activeHeight(), 4,
                                             m_imageData.data(), m_storageInfo.storedWidth() * sizeof(uint32_t));
        if(!result)
            throw std::runtime_error("stbi_write_png_to_func has failed");

        return output;
    }

    int ExtractedTextureImage::getPNGCompressionLevel() {
        return stbi_write_png_compression_level;
    }

    void ExtractedTextureImage::setPNGCompressionLevel(int level) {
        stbi_write_png_compression_level = level;
    }

}
