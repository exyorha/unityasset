#ifndef STB_IMAGE_WRITE_CONFIG_H
#define STB_IMAGE_WRITE_CONFIG_H

#include <cstring>
#include <cstdint>

unsigned char *stb_image_write_zlib_compress(unsigned char *data, int data_len, int *out_len, int quality);
uint32_t stb_image_write_crc32(const unsigned char *buffer, size_t len);

#define STBIW_ZLIB_COMPRESS stb_image_write_zlib_compress
#define STBI_WRITE_NO_STDIO
#define STBIW_CRC32 stb_image_write_crc32

#include "stb_image_write.h"

#endif
