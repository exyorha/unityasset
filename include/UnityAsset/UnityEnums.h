#ifndef UNITY_ASSET_UNITY_ENUMS_H
#define UNITY_ASSET_UNITY_ENUMS_H

#include <cstdint>

namespace UnityAsset {

    enum class IndexFormat: int32_t {
        UInt16 = 0,
        UInt32 = 1,
    };

    enum class MeshTopology: int32_t {
        Triangles = 0,
        Quads = 2,
        Lines = 3,
        LineStrip = 4,
        Points = 5,
    };

    enum class VertexAttributeFormat: int32_t {
        Float32 = 0,
        Float16 = 1,
        UNorm8 = 2,
        SNorm8 = 3,
        UNorm16 = 4,
        SNorm16 = 5,
        UInt8 = 6,
        SInt8 = 7,
        UInt16 = 8,
        SInt16 = 9,
        UInt32 = 10,
        SInt32 = 11,
    };

    enum class VertexAttribute: int32_t {
        Position = 0,
        Normal = 1,
        Tangent = 2,
        Color = 3,
        TexCoord0 = 4,
        TexCoord1 = 5,
        TexCoord2 = 6,
        TexCoord3 = 7,
        TexCoord4 = 8,
        TexCoord5 = 9,
        TexCoord6 = 10,
        TexCoord7 = 11,
        BlendWeight = 12,
        BlendIndices = 13,
    };
}

#endif

