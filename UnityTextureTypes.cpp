#include <UnityAsset/UnityTextureTypes.h>
#include <UnityAsset/UnityTypes.h>

#include <string>
#include <stdexcept>

namespace UnityAsset {

    const TextureFormatClassification TextureFormatClassification::RGBA8(TextureEncodingClass::RGBA8, 1, 1);
    const TextureFormatClassification TextureFormatClassification::RGBA32F(TextureEncodingClass::RGBA32F, 1, 1);
    const TextureFormatClassification TextureFormatClassification::RGBA16F(TextureEncodingClass::RGBA16F, 1, 1);
    const TextureFormatClassification TextureFormatClassification::DXT1(TextureEncodingClass::DXT1, 4, 4);
    const TextureFormatClassification TextureFormatClassification::DXT5(TextureEncodingClass::DXT5, 4, 4);
    const TextureFormatClassification TextureFormatClassification::BC7(TextureEncodingClass::BC7, 4, 4);

    TextureFormatClassification::TextureFormatClassification(TextureEncodingClass encodingClass,
                                                             unsigned int blockWidth, unsigned int blockHeight) :
                                                             m_encodingClass(encodingClass),
        m_blockWidth(blockWidth), m_blockHeight(blockHeight) {

    }

    TextureFormatClassification::~TextureFormatClassification() = default;

    TextureFormatClassification::TextureFormatClassification(const TextureFormatClassification &other) = default;

    TextureFormatClassification &TextureFormatClassification::operator =(const TextureFormatClassification &other) = default;

    TextureFormatClassification TextureFormatClassification::classify(TextureFormat format) {
        switch(format) {
            case TextureFormat::Alpha8:
                return TextureFormatClassification(TextureEncodingClass::A8, 1, 1);

            case RGB24:
                return TextureFormatClassification(TextureEncodingClass::RGB8, 1, 1);

            case RGBA32:
                return TextureFormatClassification(TextureEncodingClass::RGBA8, 1, 1);

            case ARGB32:
                return TextureFormatClassification(TextureEncodingClass::ARGB8, 1, 1);

            case RGBA4444:
                return TextureFormatClassification(TextureEncodingClass::RGBA4444, 1, 1);

            case RGBAFloat:
                return TextureFormatClassification(TextureEncodingClass::RGBA32F, 1, 1);

            case ETC_RGB4:
                return TextureFormatClassification(TextureEncodingClass::ETC1, 4, 4);

            case ETC2_RGBA8:
                return TextureFormatClassification(TextureEncodingClass::ETC2_RGBA, 4, 4);

            case ASTC_RGB_4x4:
            case ASTC_RGBA_4x4:
                return TextureFormatClassification(TextureEncodingClass::ASTC_LDR, 4, 4);

            case ASTC_RGB_5x5:
            case ASTC_RGBA_5x5:
                return TextureFormatClassification(TextureEncodingClass::ASTC_LDR, 5, 5);

            case TextureFormat::ASTC_RGB_6x6:
            case TextureFormat::ASTC_RGBA_6x6:
                return TextureFormatClassification(TextureEncodingClass::ASTC_LDR, 6, 6);

            case TextureFormat::ASTC_RGB_8x8:
            case TextureFormat::ASTC_RGBA_8x8:
                return TextureFormatClassification(TextureEncodingClass::ASTC_LDR, 8, 8);

            case TextureFormat::ASTC_RGB_10x10:
            case TextureFormat::ASTC_RGBA_10x10:
                return TextureFormatClassification(TextureEncodingClass::ASTC_LDR, 10, 10);

            case TextureFormat::ASTC_RGB_12x12:
            case TextureFormat::ASTC_RGBA_12x12:
                return TextureFormatClassification(TextureEncodingClass::ASTC_LDR, 12, 12);

            case TextureFormat::R8:
                return TextureFormatClassification(TextureEncodingClass::R8, 1, 1);

            case TextureFormat::ASTC_HDR_4x4:
                return TextureFormatClassification(TextureEncodingClass::ASTC_HDR, 4, 4);

            case TextureFormat::ASTC_HDR_5x5:
                return TextureFormatClassification(TextureEncodingClass::ASTC_HDR, 5, 5);

            case TextureFormat::ASTC_HDR_6x6:
                return TextureFormatClassification(TextureEncodingClass::ASTC_HDR, 6, 6);

            case TextureFormat::ASTC_HDR_8x8:
                return TextureFormatClassification(TextureEncodingClass::ASTC_HDR, 8, 8);

            case TextureFormat::ASTC_HDR_10x10:
                return TextureFormatClassification(TextureEncodingClass::ASTC_HDR, 10, 10);

            case TextureFormat::ASTC_HDR_12x12:
                return TextureFormatClassification(TextureEncodingClass::ASTC_HDR, 12, 12);

            default:
                throw std::runtime_error("unsupported Unity texture format " + std::to_string(static_cast<int32_t>(format)));
        }
    }

    ImageStorageInfo TextureFormatClassification::determineLayout(unsigned int width, unsigned int height) const {
        unsigned int blocksX = (width + (m_blockWidth - 1)) / m_blockWidth;
        unsigned int blocksY = (height + (m_blockHeight - 1)) / m_blockHeight;

        unsigned int bytes = blocksX * blocksY * blockSizeBytes();

        unsigned int alignedWidth = blocksX * m_blockWidth;
        unsigned int alignedHeight = blocksY * m_blockHeight;

        return ImageStorageInfo(width, height, alignedWidth, alignedHeight, bytes);
    }

    ImageStorageInfo TextureFormatClassification::determineLayout(unsigned int width, unsigned int height,
                                                                  unsigned int paddedWidth, unsigned int paddedHeight) const {

        if(paddedWidth < width || (paddedWidth % m_blockWidth) != 0 || paddedHeight < height || (paddedHeight % m_blockHeight) != 0) {
            throw std::runtime_error("incorrect padded image size specification");
        }

        unsigned int blocksX = paddedWidth / m_blockWidth;
        unsigned int blocksY = paddedHeight / m_blockHeight;

        unsigned int bytes = blocksX * blocksY * blockSizeBytes();

        return ImageStorageInfo(width, height, paddedWidth, paddedHeight, bytes);

    }

    TextureFormat TextureFormatClassification::unityFormat() const {
        switch(m_encodingClass) {
            case UnityAsset::TextureEncodingClass::DXT1:
                return TextureFormat::DXT1;

            case UnityAsset::TextureEncodingClass::DXT5:
                return TextureFormat::DXT5;

            case UnityAsset::TextureEncodingClass::BC7:
                return TextureFormat::BC7;

            case UnityAsset::TextureEncodingClass::RGBA16F:
                return TextureFormat::RGBAHalf;

        default:
            throw std::runtime_error("cannot find a suitable Unity texture format for this format classification");
        }
    }

    unsigned int TextureFormatClassification::blockSizeBytes() const {
        switch(m_encodingClass) {
            case UnityAsset::TextureEncodingClass::A8:
            case UnityAsset::TextureEncodingClass::R8:
                return 1;

            case UnityAsset::TextureEncodingClass::DXT1:
                return 8;

            case UnityAsset::TextureEncodingClass::DXT5:
                return 16;

            case UnityAsset::TextureEncodingClass::BC7:
                return 16;

            case UnityAsset::TextureEncodingClass::ETC1:
                return 8;

            case UnityAsset::TextureEncodingClass::ETC2_RGBA:
                return 16;

            case UnityAsset::TextureEncodingClass::ASTC_LDR:
            case UnityAsset::TextureEncodingClass::ASTC_HDR:
                return 16;

            case UnityAsset::TextureEncodingClass::RGB8:
                return 3;

            case UnityAsset::TextureEncodingClass::ARGB8:
            case UnityAsset::TextureEncodingClass::RGBA8:
                return 4;

            case UnityAsset::TextureEncodingClass::RGBA4444:
                return 2;

            case UnityAsset::TextureEncodingClass::RGBA32F:
                return 16;

            case UnityAsset::TextureEncodingClass::RGBA16F:
                return 8;

            default:
                throw std::logic_error("unsupported encoding class");
        }
    }

    TextureImageLayout::TextureImageLayout(const UnityAsset::UnityTypes::Texture2D &texture) :
        m_format(TextureFormatClassification::classify(static_cast<TextureFormat>(texture.m_TextureFormat))) {

        printf("2D texture '%s': %dx%d with %d mipmaps, format %d, total %d images, total %d bytes\n",
               texture.m_Name.c_str(),
               texture.m_Width, texture.m_Height, texture.m_MipCount, texture.m_TextureFormat, texture.m_ImageCount, texture.m_CompleteImageSize);

        unsigned int runningOffset = 0;

        if(texture.m_Width == 0 && texture.m_Height == 0 && texture.m_MipCount == 1 && (texture.m_ImageCount == 0 || texture.m_ImageCount == 1)) {
            /*
             * Empty texture, no images.
             */
        } else {
            for(int32_t mip = 0; mip < texture.m_MipCount; mip++) {
                auto widthAtMip = std::max<int32_t>(1, texture.m_Width >> mip);
                auto heightAtMip = std::max<int32_t>(1, texture.m_Height >> mip);

                auto layout = m_format.determineLayout(widthAtMip, heightAtMip);

                auto &image = m_images.emplace_back(layout, runningOffset);

                printf("Texture2D mip level %d: active size %ux%u, storage size %ux%u, total data length: %u, offset: %u\n",
                    mip, layout.activeWidth(), layout.activeHeight(), layout.storedWidth(), layout.storedHeight(), layout.dataLength(),
                    image.offset());

                runningOffset += image.storageInfo().dataLength();
            }

            if(texture.m_ImageCount != 1) {
                throw std::runtime_error("the meaning of 2D textures with more than one image is unknown");
            }
        }

        if(static_cast<int32_t>(runningOffset) != texture.m_ImageCount * texture.m_CompleteImageSize) {
            throw std::runtime_error("the complete image size in the asset data doesn't match the total image size");
        }


        m_totalDataSize = runningOffset;
    }

    TextureImageLayout::TextureImageLayout(const UnityAsset::UnityTypes::Texture3D &texture) :
        m_format(TextureFormatClassification::classify(static_cast<TextureFormat>(texture.m_Format))) {

        throw std::logic_error("Texture3D is not implemented");

    }
    TextureImageLayout::TextureImageLayout(const UnityAsset::UnityTypes::Texture2DArray &texture) :
        m_format(TextureFormatClassification::classify(static_cast<TextureFormat>(texture.m_Format))) {

        throw std::logic_error("Texture2DArray is not implemented");
    }

    TextureImageLayout::TextureImageLayout(const UnityAsset::UnityTypes::Cubemap &texture) :
        m_format(TextureFormatClassification::classify(static_cast<TextureFormat>(texture.m_TextureFormat))) {

        printf("Cubemap '%s': %dx%d with %d mipmaps, format %d, total %d images, total %d bytes\n",
               texture.m_Name.c_str(),
               texture.m_Width, texture.m_Height, texture.m_MipCount, texture.m_TextureFormat, texture.m_ImageCount, texture.m_CompleteImageSize);

        size_t runningOffset = 0;

        for(unsigned int face = 0; face < 6; face++) {
            for(int32_t mip = 0; mip < texture.m_MipCount; mip++) {
                auto widthAtMip = std::max<int32_t>(1, texture.m_Width >> mip);
                auto heightAtMip = std::max<int32_t>(1, texture.m_Height >> mip);

                auto layout = m_format.determineLayout(widthAtMip, heightAtMip);

                auto &image = m_images.emplace_back(layout, runningOffset);

                printf("cubemap face %u mip level %d: active size %ux%u, storage size %ux%u, total data length: %u, offset: %u\n",
                       face,
                    mip, layout.activeWidth(), layout.activeHeight(), layout.storedWidth(), layout.storedHeight(), layout.dataLength(),
                    image.offset());

                runningOffset += image.storageInfo().dataLength();
            }

        }

        if(texture.m_ImageCount != 6) {
            throw std::runtime_error("the meaning of 2D textures with any number of images other than 6e is unknown");
        }

        if(static_cast<int32_t>(runningOffset) != texture.m_ImageCount * texture.m_CompleteImageSize) {
            throw std::runtime_error("the complete image size in the asset data doesn't match the total image size");
        }

        m_totalDataSize = runningOffset;
    }
    TextureImageLayout::TextureImageLayout(const UnityAsset::UnityTypes::CubemapArray &texture) :
        m_format(TextureFormatClassification::classify(static_cast<TextureFormat>(texture.m_Format))) {

        throw std::logic_error("CubemapArray is not implemented");
    }

    TextureImageLayout::~TextureImageLayout() = default;

    TextureImageLayout::TextureImageLayout(TextureImageLayout &&other) noexcept = default;

    TextureImageLayout &TextureImageLayout::operator =(TextureImageLayout &&other) noexcept = default;

    TextureImageLayout::TextureImageLayout(const TextureImageLayout &sourceLayout, const TextureFormatClassification &newFormat, bool maintainPadding) :
        m_format(newFormat) {

        size_t runningOffset = 0;

        for(const auto &sourceImage: sourceLayout.images()) {


            if(maintainPadding) {
                auto &image = m_images.emplace_back(m_format.determineLayout(
                    sourceImage.storageInfo().activeWidth(),
                    sourceImage.storageInfo().activeHeight(),
                    sourceImage.storageInfo().storedWidth(),
                    sourceImage.storageInfo().storedHeight()),
                    runningOffset);

                runningOffset += image.storageInfo().dataLength();
            } else {
                auto &image = m_images.emplace_back(m_format.determineLayout(
                    sourceImage.storageInfo().activeWidth(),
                    sourceImage.storageInfo().activeHeight()),
                    runningOffset);

                runningOffset += image.storageInfo().dataLength();
            }
        }

        m_totalDataSize = runningOffset;
    }

    TextureSubImage::TextureSubImage(const ImageStorageInfo &storageInfo, unsigned int offset) : m_storageInfo(storageInfo), m_offset(offset) {

    }

    TextureSubImage::~TextureSubImage() = default;

    TextureSubImage::TextureSubImage(const TextureSubImage &other) = default;

    TextureSubImage &TextureSubImage::operator =(const TextureSubImage &other) = default;

    ImageStorageInfo::ImageStorageInfo(
        unsigned int activeWidth, unsigned int activeHeight,
        unsigned int storedWidth, unsigned int storedHeight,
        unsigned int dataLength) : m_activeWidth(activeWidth), m_activeHeight(activeHeight),
        m_storedWidth(storedWidth), m_storedHeight(storedHeight), m_dataLength(dataLength) {

    }

    ImageStorageInfo::~ImageStorageInfo() = default;

    ImageStorageInfo::ImageStorageInfo(const ImageStorageInfo &other) = default;

    ImageStorageInfo &ImageStorageInfo::operator =(const ImageStorageInfo &other) = default;


}

