#include <UnityAsset/UnityCompression.h>

#include <UnityAsset/Streams/Stream.h>
#include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>

#include <lz4.h>
#include <lz4hc.h>

#include <UnityAsset/UnityAssetConfig.h>

#ifdef LibLZMA_FOUND
#include <lzma.h>
#else
#include "lzmadec/LzmaDec.h"
#endif

#include <array>

namespace UnityAsset {

#ifndef LibLZMA_FOUND

    static void *LzmaAllocatorAlloc(ISzAllocPtr p, size_t size) {
        (void)p;

        return malloc(size);
    }

    static void LzmaAllocatorFree(ISzAllocPtr p, void *address) {
        (void)p;

        free(address);
    }

    static const ISzAlloc LzmaAllocator = {
        .Alloc = LzmaAllocatorAlloc,
        .Free = LzmaAllocatorFree
    };
#endif

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

            case UnityAsset::UnityCompressionType::LZMA:
            {

#ifdef LibLZMA_FOUND
                std::array<lzma_filter, 2> filters{ {
                    { .id = LZMA_FILTER_LZMA1, .options = nullptr },
                    { .id = LZMA_VLI_UNKNOWN, .options = nullptr },
                } };

                struct OptionsReleaser {
                    OptionsReleaser(void **options) : options(options) {

                    }

                    ~OptionsReleaser() {
                        free(*options);
                    }

                private:
                    void **options;
                } optionsReleaser(&filters[0].options);

                uint32_t proplength;
                auto result = lzma_properties_size(&proplength, filters.data());
                if(result != LZMA_OK)
                    throw std::runtime_error("lzma_properties_size has failed");

                if(inputLength < proplength) {
                    throw std::runtime_error("the LZMA input data doesn't fit the properties");
                }

                result = lzma_properties_decode(filters.data(), nullptr, inputData, proplength);
                if(result != LZMA_OK) {
                    throw std::runtime_error("failed to decode the LZMA properties");
                }

                size_t inputPos = proplength;
                size_t outputPos = 0;

                result = lzma_raw_buffer_decode(filters.data(), nullptr,
                                                inputData, &inputPos, inputLength, outputData, &outputPos, outputLength);
                if(result  != LZMA_OK)
                    throw std::runtime_error("failed to decode the LZMA data");

                if(outputPos != outputLength || inputPos != inputLength)
                    throw std::runtime_error("mismatched LZMA data length");
#else

                if(inputLength < LZMA_PROPS_SIZE) {
                    throw std::runtime_error("the LZMA input data doesn't fit the properties");
                }


                SizeT producedOutputLength = outputLength;
                SizeT consumedInputLength = inputLength - LZMA_PROPS_SIZE;;
                ELzmaStatus status;

                auto result = LzmaDecode(outputData, &producedOutputLength, inputData + LZMA_PROPS_SIZE, &consumedInputLength,
                                         inputData, LZMA_PROPS_SIZE, LZMA_FINISH_END, &status, &LzmaAllocator);
                if(result != SZ_OK)
                    throw std::runtime_error("failed to decode the LZMA stream: " + std::to_string(result));

                if(status != LZMA_STATUS_FINISHED_WITH_MARK)
                    throw std::runtime_error("no proper signaling at the end of the LZMA stream");

                if(producedOutputLength != outputLength || consumedInputLength != inputLength - LZMA_PROPS_SIZE)
                    throw std::runtime_error("mismatched LZMA data length");

#endif
                break;
            }

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

#ifdef LibLZMA_FOUND
            case UnityCompressionType::LZMA:
            {
                lzma_options_lzma options;
                if(lzma_lzma_preset(&options, 5))
                    throw std::runtime_error("lzma_lzma_preset has failed");

                std::array<lzma_filter, 2> filters{ {
                    { .id = LZMA_FILTER_LZMA1, .options = &options },
                    { .id = LZMA_VLI_UNKNOWN, .options = nullptr },
                } };

                uint32_t proplength;
                auto result = lzma_properties_size(&proplength, filters.data());
                if(result != LZMA_OK)
                    throw std::runtime_error("lzma_properties_size has failed");

                size_t prefix = proplength;

                if(prefix >= inputLength) {
                    return false;
                }

                result = lzma_properties_encode(filters.data(), outputData);
                if(result != LZMA_OK)
                    throw std::runtime_error("lzma_properties_encode has failed");

                outputLength = prefix;

                result = lzma_raw_buffer_encode(filters.data(), nullptr, inputData, inputLength, outputData, &outputLength, inputLength);
                if(result == LZMA_BUF_ERROR) {
                    return false;
                }

                if(result == LZMA_OK) {
                    printf("LZMA: compressed %zu bytes to %zu bytes\n", inputLength, outputLength);
                    return true;
                }

                throw std::runtime_error("lzma_raw_buffer_encode has failed: " + std::to_string(result));
            }
#endif

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
