#ifndef TEXCOMPRESS_ASTC_H
#define TEXCOMPRESS_ASTC_H

#include <cstdint>

void
unpack_astc_2d_ldr(uint8_t *dst_row,
                   unsigned dst_stride,
                   const uint8_t *src_row,
                   unsigned src_stride,
                   unsigned src_width,
                   unsigned src_height,
                   unsigned blk_w,
                   unsigned blk_h,
                   bool srgb);

#endif

