#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write_config.h"

#include <zlib.h>

unsigned char *stb_image_write_zlib_compress(unsigned char *data, int data_len, int *out_len, int quality) {
    auto bufferLength = compressBound(data_len);

    auto buffer = static_cast<Bytef *>(STBIW_MALLOC(bufferLength));
    if(!buffer)
        return nullptr;

    uLongf outputLength = bufferLength;

    auto result = compress2(buffer, &outputLength, data, data_len, quality);
    if(result != Z_OK) {
        STBIW_FREE(buffer);
        return nullptr;
    }

    *out_len = outputLength;

    return buffer;
}
