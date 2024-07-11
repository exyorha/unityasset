/*
 * Based on code for half-precision float to single-precision float
 * conversion by Fabian "ryg" Giesen, placed in the public domain by
 * the original author.
 */

#include <UnityAsset/HalfFloat.h>

#include <bit>

namespace {
    using uint = unsigned int;

    union FP32
    {
        uint u;
        float f;
        struct
        {
            uint Mantissa : 23;
            uint Exponent : 8;
            uint Sign : 1;
        };
    };
    static_assert(sizeof(FP32) == 4);

    union FP16
    {
        unsigned short u;
        struct
        {
            unsigned short Mantissa : 10;
            unsigned short Exponent : 5;
            unsigned short Sign : 1;
        };
    };
    static_assert(sizeof(FP16) == 2);
}

namespace UnityAsset {

    float halfFloatToFloat(uint16_t h) {
        static constexpr uint32_t magic = { 113 << 23 };
        static constexpr uint32_t shifted_exp = 0x7c00 << 13; // exponent mask after shift
        uint32_t o;

        // mantissa+exponent
        uint32_t shifted = static_cast<uint32_t>(h & 0x7fff) << 13;
        uint32_t exponent = shifted & shifted_exp;

        // exponent cases
        o = shifted;
        if (exponent == 0) // Zero / Denormal
        {
            o += magic;
            o = std::bit_cast<uint32_t>(std::bit_cast<float>(o) - std::bit_cast<float>(magic));
        }
        else if (exponent == shifted_exp) // Inf/NaN
            o += (255 - 31) << 23;
        else
            o += (127 - 15) << 23;

        o |= static_cast<uint32_t>(h & 0x8000) << 16; // copy sign bit
        return std::bit_cast<float>(o);
    }

    /*
    * This is ryg's float_to_half_fast3_rtne; [fast], but rounding ties to nearest even instead of towards +inf
    */
    static FP16 float_to_half_fast3_rtne(FP32 f)
    {
        FP32 f32infty = { 255 << 23 };
        FP32 f16max   = { (127 + 16) << 23 };
        FP32 denorm_magic = { ((127 - 15) + (23 - 10) + 1) << 23 };
        uint sign_mask = 0x80000000u;
        FP16 o = { 0 };

        uint sign = f.u & sign_mask;
        f.u ^= sign;

        // NOTE all the integer compares in this function can be safely
        // compiled into signed compares since all operands are below
        // 0x80000000. Important if you want fast straight SSE2 code
        // (since there's no unsigned PCMPGTD).

        if (f.u >= f16max.u) // result is Inf or NaN (all exponent bits set)
            o.u = (f.u > f32infty.u) ? 0x7e00 : 0x7c00; // NaN->qNaN and Inf->Inf
        else // (De)normalized number or zero
        {
            if (f.u < (113 << 23)) // resulting FP16 is subnormal or zero
            {
                // use a magic value to align our 10 mantissa bits at the bottom of
                // the float. as long as FP addition is round-to-nearest-even this
                // just works.
                f.f += denorm_magic.f;

                // and one integer subtract of the bias later, we have our final float!
                o.u = f.u - denorm_magic.u;
            }
            else
            {
                uint mant_odd = (f.u >> 13) & 1; // resulting mantissa is odd

                // update exponent, rounding bias part 1
                f.u += ((15 - 127) << 23) + 0xfff;
                // rounding bias part 2
                f.u += mant_odd;
                // take the bits!
                o.u = f.u >> 13;
            }
        }

        o.u |= sign >> 16;
        return o;
    }

    uint16_t floatToHalfFloat(float value) {
        FP32 u;
        u.f = value;

        return float_to_half_fast3_rtne(u).u;
    }

}
