#include <UnityAsset/UnityCompression.h>

#include <UnityAsset/Streams/Stream.h>
#include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>

#include <lz4.h>
#include <lz4hc.h>

namespace UnityAsset {

    Stream unityUncompress(Stream &&input, UnityCompressionType compression, size_t uncompressedLength) {
        if(compression == UnityCompressionType::None && uncompressedLength == input.length())
            return input;

        std::vector<unsigned char> outputData(uncompressedLength);

        unityUncompress(input.data(), input.length(), compression, outputData.data(), outputData.size());

        Stream outputStream(std::make_shared<InMemoryStreamBackingBuffer>(std::move(outputData)));
        return outputStream;
    }


    void unityUncompress(const unsigned char *inputData, size_t inputLength, UnityCompressionType compression, unsigned char *outputData, size_t outputLength) {

        switch(compression) {
            case UnityCompressionType::None:
                if(inputLength != outputLength)
                    throw std::runtime_error("unityUncompress: the output length doesn't equal the input length for the uncompressed data");

                memcpy(outputData, inputData, inputLength);

                break;

            case UnityCompressionType::LZ4:
            case UnityCompressionType::LZ4HC:
            {
                auto result = LZ4_decompress_safe(
                    reinterpret_cast<const char *>(inputData),
                    reinterpret_cast<char *>(outputData),
                    inputLength,
                    outputLength);
                if(result < 0)
                    throw std::runtime_error("LZ4 decompression has failed");

                if(static_cast<size_t>(result) != outputLength)
                    throw std::runtime_error("unexpected length of the output data");

                break;
            }

            default:
                throw std::runtime_error("unityUncompress: unsupported compression: " + std::to_string(static_cast<uint32_t>(compression)));
        }
    }

    bool unityCompress(const unsigned char *inputData, size_t inputLength, UnityCompressionType compression, unsigned char *outputData, size_t &outputLength) {
        switch(compression) {
            case UnityCompressionType::None:
                break;

            case UnityCompressionType::LZ4:
            {
                auto result = LZ4_compress_default(
                    reinterpret_cast<const char *>(inputData),
                    reinterpret_cast<char *>(outputData),
                    inputLength,
                    inputLength);
                if(result < 0)
                    throw std::runtime_error("LZ4 compression has failed");

                if(result > 0) {
                    outputLength = static_cast<size_t>(result);
                    return true;
                }

                break;
            }


            case UnityCompressionType::LZ4HC:
            {
                auto result = LZ4_compress_HC(
                    reinterpret_cast<const char *>(inputData),
                    reinterpret_cast<char *>(outputData),
                    inputLength,
                    inputLength,
                    LZ4HC_CLEVEL_MAX);
                if(result < 0)
                    throw std::runtime_error("LZ4 compression has failed");

                if(result > 0) {
                    outputLength = static_cast<size_t>(result);
                    return true;
                }

                break;
            }

            default:
                throw std::runtime_error("unityCompress: unsupported compression: " + std::to_string(static_cast<uint32_t>(compression)));
        }

        memcpy(outputData, inputData, inputLength);
        outputLength = inputLength;
        return false;
    }
}
