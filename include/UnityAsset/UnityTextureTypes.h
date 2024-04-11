#ifndef UNITY_ASSET_UNITY_TEXTURE_TYPES_H
#define UNITY_ASSET_UNITY_TEXTURE_TYPES_H

#include <cstdint>
#include <vector>

namespace UnityAsset {

    namespace UnityTypes {
        struct Texture2D;
        struct Texture2DArray;
        struct Texture3D;
        struct Cubemap;
        struct CubemapArray;
    };

    enum TextureFormat: int32_t {
        Alpha8 = 1,
        ARGB4444 = 2,
        RGB24 = 3,
        RGBA32 = 4,
        ARGB32 = 5,
        RGB565 = 7,
        R16 = 9,
        DXT1 = 10,
        DXT5 = 12,
        RGBA4444 = 13,
        BGRA32 = 14,
        RHalf = 15,
        RGHalf = 16,
        RGBAHalf = 17,
        RFloat = 18,
        RGFloat = 19,
        RGBAFloat = 20,
        YUY2 = 21,
        RGB9e5Float = 22,
        BC4 = 26,
        BC5 = 27,
        BC6H = 24,
        BC7 = 25,
        DXT1Crunched = 28,
        DXT5Crunched = 29,
        PVRTC_RGB2 = 30,
        PVRTC_RGBA2 = 31,
        PVRTC_RGB4 = 32,
        PVRTC_RGBA4 = 33,
        ETC_RGB4 = 34,
        EAC_R = 41,
        EAC_R_SIGNED = 42,
        EAC_RG = 43,
        EAC_RG_SIGNED = 44,
        ETC2_RGB = 45,
        ETC2_RGBA1 = 46,
        ETC2_RGBA8 = 47,
        ASTC_4x4 = 48,
        ASTC_5x5 = 49,
        ASTC_6x6 = 50,
        ASTC_8x8 = 51,
        ASTC_10x10 = 52,
        ASTC_12x12 = 53,
        ETC_RGB4_3DS = 60,
        ETC_RGBA8_3DS = 61,
        RG16 = 62,
        R8 = 63,
        ETC_RGB4Crunched = 64,
        ETC2_RGBA8Crunched = 65,
        ASTC_HDR_4x4 = 66,
        ASTC_HDR_5x5 = 67,
        ASTC_HDR_6x6 = 68,
        ASTC_HDR_8x8 = 69,
        ASTC_HDR_10x10 = 70,
        ASTC_HDR_12x12 = 71,
        RG32 = 72,
        RGB48 = 73,
        RGBA64 = 74,
        ASTC_RGB_4x4 = 48,
        ASTC_RGB_5x5 = 49,
        ASTC_RGB_6x6 = 50,
        ASTC_RGB_8x8 = 51,
        ASTC_RGB_10x10 = 52,
        ASTC_RGB_12x12 = 53,
        ASTC_RGBA_4x4 = 54,
        ASTC_RGBA_5x5 = 55,
        ASTC_RGBA_6x6 = 56,
        ASTC_RGBA_8x8 = 57,
        ASTC_RGBA_10x10 = 58,
        ASTC_RGBA_12x12 = 59,
    };

    enum class TextureEncodingClass : uint8_t {
        /*
         * Uncompressed formats good everywhere
         */
        A8,
        R8,
        RGB8,
        RGBA8,
        ARGB8,
        RGBA4444,
        RGBA32F,
        RGBA16F,

        /*
         * Desktop compressed formats
         */
        DXT1,
        DXT5,
        BC7,

        /*
         * Mobile compressed formats
         */
        ETC1,
        ETC2_RGBA,
        ASTC_LDR,
        ASTC_HDR
    };

    class ImageStorageInfo {
    public:
        ImageStorageInfo(
            unsigned int activeWidth, unsigned int activeHeight,
            unsigned int storedWidth, unsigned int storedHeight,
            unsigned int dataLength);

        ~ImageStorageInfo();

        ImageStorageInfo(const ImageStorageInfo &other);
        ImageStorageInfo &operator =(const ImageStorageInfo &other);

        inline unsigned int activeWidth() const {
            return m_activeWidth;
        }

        inline unsigned int activeHeight() const {
            return m_activeHeight;
        }

        inline unsigned int storedWidth() const {
            return m_storedWidth;
        }

        inline unsigned int storedHeight() const {
            return m_storedHeight;
        }

        inline unsigned int dataLength() const {
            return m_dataLength;
        }

    private:
        unsigned int m_activeWidth;
        unsigned int m_activeHeight;
        unsigned int m_storedWidth;
        unsigned int m_storedHeight;
        unsigned int m_dataLength;
    };

    class TextureFormatClassification {
    private:
        TextureFormatClassification(TextureEncodingClass encodingClass, unsigned int blockWidth, unsigned int blockHeight);

    public:
        ~TextureFormatClassification();

        TextureFormatClassification(const TextureFormatClassification &other);
        TextureFormatClassification &operator =(const TextureFormatClassification &other);

        inline TextureEncodingClass encodingClass() const {
            return m_encodingClass;
        }

        inline unsigned int blockWidth() const {
            return m_blockWidth;
        }

        inline unsigned int blockHeight() const {
            return m_blockHeight;
        }

        unsigned int blockSizeBytes() const;

        static TextureFormatClassification classify(TextureFormat format);

        ImageStorageInfo determineLayout(unsigned int width, unsigned int height) const;
        ImageStorageInfo determineLayout(unsigned int width, unsigned int height, unsigned int paddedWidth, unsigned int paddedHeight) const;

        TextureFormat unityFormat() const;

        static const TextureFormatClassification RGBA8;
        static const TextureFormatClassification RGBA32F;
        static const TextureFormatClassification RGBA16F;
        static const TextureFormatClassification DXT1;
        static const TextureFormatClassification DXT5;
        static const TextureFormatClassification BC7;

    private:
        TextureEncodingClass m_encodingClass;
        uint8_t m_blockWidth;
        uint8_t m_blockHeight;
    };

    class TextureSubImage {
    public:
        TextureSubImage(const ImageStorageInfo &storageInfo, unsigned int offset);
        ~TextureSubImage();

        TextureSubImage(const TextureSubImage &other);
        TextureSubImage &operator =(const TextureSubImage &other);

        inline const ImageStorageInfo &storageInfo() const {
            return m_storageInfo;
        }

        inline unsigned int offset() const {
            return m_offset;
        }

    private:
        ImageStorageInfo m_storageInfo;
        unsigned int m_offset;
    };

    class TextureImageLayout {
    private:
        TextureImageLayout(const TextureImageLayout &sourceLayout, const TextureFormatClassification &newFormat, bool maintainPadding);

    public:

        explicit TextureImageLayout(const UnityAsset::UnityTypes::Texture2D &texture);
        explicit TextureImageLayout(const UnityAsset::UnityTypes::Texture3D &texture);
        explicit TextureImageLayout(const UnityAsset::UnityTypes::Texture2DArray &texture);
        explicit TextureImageLayout(const UnityAsset::UnityTypes::Cubemap &texture);
        explicit TextureImageLayout(const UnityAsset::UnityTypes::CubemapArray &texture);
        ~TextureImageLayout();

        TextureImageLayout(const TextureImageLayout &other) = delete;
        TextureImageLayout &operator =(const TextureImageLayout &other) = delete;

        TextureImageLayout(TextureImageLayout &&other) noexcept;
        TextureImageLayout &operator =(TextureImageLayout &&other) noexcept;

        inline const TextureFormatClassification &format() const {
            return m_format;
        }

        inline const std::vector<TextureSubImage> &images() const {
            return m_images;
        }

        inline size_t totalDataSize() const {
            return m_totalDataSize;
        }

        inline TextureImageLayout reformat(const TextureFormatClassification &format, bool maintainPadding = false) const {
            return TextureImageLayout(*this, format, maintainPadding);
        }

    private:
        TextureFormatClassification m_format;
        std::vector<TextureSubImage> m_images;
        size_t m_totalDataSize;
    };

    enum ColorSpace: int32_t {
        Uninitialized = -1,
        Gamma = 0,
        Linear = 1,
    };
}

#endif
