#ifndef STB_IMAGE_WRITE_CONFIG_H
#define STB_IMAGE_WRITE_CONFIG_H

unsigned char *stb_image_write_zlib_compress(unsigned char *data, int data_len, int *out_len, int quality);

#define STBIW_ZLIB_COMPRESS stb_image_write_zlib_compress
#define STBI_WRITE_NO_STDIO

#include "stb_image_write.h"

#endif
