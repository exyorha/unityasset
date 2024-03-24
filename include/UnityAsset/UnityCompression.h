#ifndef UNITY_ASSET_UNITY_COMPRESSION_H
#define UNITY_ASSET_UNITY_COMPRESSION_H

#include <cstdint>
#include <cstring>

namespace UnityAsset {

    class Stream;

    enum class UnityCompressionType : uint32_t {
        None  = 0,
        LZMA  = 1,
        LZ4   = 2,
        LZ4HC = 3,
        LZHAM = 4
    };

    Stream unityUncompress(Stream &&input, UnityCompressionType compression, size_t uncompressedLength);
    void unityUncompress(const unsigned char *inputData, size_t inputLength, UnityCompressionType compression, unsigned char *outputData, size_t outputLength);
    bool unityCompress(const unsigned char *inputData, size_t inputLength, UnityCompressionType compression, unsigned char *outputData, size_t &outputLength);
}

#endif

