#ifndef UNITY_ASSET_HALF_FLOAT_H
#define UNITY_ASSET_HALF_FLOAT_H

#include <cstdint>

namespace UnityAsset {

    float halfFloatToFloat(uint16_t halfFloat);
    uint16_t floatToHalfFloat(float value);

    struct HalfFloat {
        uint16_t value;

        inline operator float() const {
            return halfFloatToFloat(value);
        }

    };

}

#endif
