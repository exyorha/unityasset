#include "UnityAsset/UnityCompression.h"
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h>
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleDirectory.h>

#include <UnityAsset/Streams/Stream.h>
#include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>

#include <zlib.h>

#include <limits>

#include <algorithm>
#include <execution>

namespace UnityAsset {

    AssetBundleFile::AssetBundleFile() : directoryCompression(UnityCompressionType::LZ4HC), dataCompression(UnityCompressionType::None),
        blockSize(128 * 1024) {

    }

    AssetBundleFile::~AssetBundleFile() = default;

    AssetBundleFile::AssetBundleFile(AssetBundleFile &&other) noexcept = default;

    AssetBundleFile &AssetBundleFile::operator =(AssetBundleFile &&other) noexcept = default;

    AssetBundleFile::AssetBundleFile(Stream &&stream) : AssetBundleFile() {
        stream.setByteOrder(Stream::ByteOrder::MostSignificantFirst);

        auto signature = stream.readNullTerminatedString();
        if(signature != AssetBundleSignature)
            throw std::runtime_error("AssetBundleFile: bad signature");

        uint32_t version;
        stream >> version;

        if(version != AssetBundleVersion)
            throw std::runtime_error("AssetBundleFile: bad version");

        unityVersion = stream.readNullTerminatedString();
        unityRevision = stream.readNullTerminatedString();

        uint64_t fileSize;
        stream >> fileSize;

        if(fileSize != stream.length())
            throw std::runtime_error("AssetBundleFile: mismatched file size");

        uint32_t compressedDirectoryLength;
        uint32_t uncompressedDirectoryLength;
        DirectoryFlags directoryFlags;
        stream
            >> compressedDirectoryLength
            >> uncompressedDirectoryLength
            >> directoryFlags;

        if(directoryCompressionOptions(directoryFlags) != BlocksAndDirectoryInfoCombined)
            throw std::runtime_error("AssetBundleFile: unsupported directory options");

        directoryCompression = directoryCompressionType(directoryFlags);
        auto compressedDirectory = stream.createView(stream.position(), compressedDirectoryLength);
        stream.setPosition(stream.position() + compressedDirectoryLength);

        auto uncompressedDirectory = unityUncompress(std::move(compressedDirectory), directoryCompression, uncompressedDirectoryLength);

        AssetBundleDirectory directory(std::move(uncompressedDirectory));
        for(auto byte: directory.uncompressedDataHash)
            if(byte != 0)
                throw std::runtime_error("AssetBundleFile: uncompressedDataHash is non-zero");

        size_t totalCompressedSize = 0;
        size_t totalUncompressedSize = 0;

        for(const auto &block: directory.blocks) {
            totalCompressedSize += block.compressedSize;
            totalUncompressedSize += block.uncompressedSize;
        }

        if(stream.position() + totalCompressedSize != stream.length())
            throw std::runtime_error("AssetBundleFile: total compressed length is inconsistent with the stream length");

        auto compressedData = stream.data() + stream.position();
        std::vector<unsigned char> uncompressedDataBuffer;
        uncompressedDataBuffer.resize(totalUncompressedSize);
        auto uncompressedData = uncompressedDataBuffer.data();

        for(const auto &block: directory.blocks) {
            if((block.flags & ~UINT16_C(0x3F)) != 0) {
                throw std::runtime_error("AssetBundleFile: unsupported block flags");
            }

            auto type = static_cast<UnityCompressionType>(block.flags & UINT16_C(0x3F));

            if(type != UnityCompressionType::None) {
                dataCompression = type;
            }

            unityUncompress(compressedData, block.compressedSize, type, uncompressedData, block.uncompressedSize);
            compressedData += block.compressedSize;
            uncompressedData += block.uncompressedSize;
        }
        assetBundleCRC.emplace(crc32(UINT32_C(0), uncompressedDataBuffer.data(), uncompressedDataBuffer.size()));

        Stream decompressedStream(std::make_shared<InMemoryStreamBackingBuffer>(std::move(uncompressedDataBuffer)));

        entries.reserve(directory.files.size());

        for(const auto &file: directory.files) {
            auto view = decompressedStream.createView(file.fileOffset, file.fileSize);

            entries.emplace_back(std::string(file.path), std::move(view), file.fileFlags);
        }
    }

    void AssetBundleFile::serialize(Stream &stream) const {
        stream.setByteOrder(Stream::ByteOrder::MostSignificantFirst);

        stream.writeNullTerminatedString(AssetBundleSignature);
        stream << AssetBundleVersion;

        stream.writeNullTerminatedString(unityVersion);
        stream.writeNullTerminatedString(unityRevision);

        /*
         * Placeholder - will be rewritten later.
         */
        uint64_t fileSize = 0;
        auto fileSizeOffset = stream.position();
        stream << fileSize;

        /*
         * Start constructing the directory.
         */
        AssetBundleDirectory directory;
        directory.files.reserve(entries.size());

        size_t totalUncompressedSize = 0;
        for(const auto &entry: entries) {
            totalUncompressedSize += (entry.data().length() + 15) & ~15;
        }

        std::vector<unsigned char> uncompressedDataBuffer(totalUncompressedSize);
        size_t position = 0;
        for(const auto &entry: entries) {
            auto &file = directory.files.emplace_back();
            file.fileOffset = position;
            file.fileSize = entry.data().length();
            file.path = entry.filename();
            file.fileFlags = entry.flags();

            memcpy(uncompressedDataBuffer.data() + position, entry.data().data(), entry.data().length());

            position += (file.fileSize + 15) & ~15;
        }

        if(assetBundleCRC.has_value()) {
            auto contentCRC = static_cast<uint32_t>(crc32(UINT32_C(0), uncompressedDataBuffer.data(), uncompressedDataBuffer.size()));

            if(contentCRC != *assetBundleCRC) {
                auto adjustment = calculateCRC32Adjustment(contentCRC, *assetBundleCRC);

                uncompressedDataBuffer.insert(uncompressedDataBuffer.end(),
                                              reinterpret_cast<const unsigned char *>(&adjustment),
                                              reinterpret_cast<const unsigned char *>(&adjustment) + sizeof(adjustment));

                contentCRC = static_cast<uint32_t>(crc32(contentCRC,
                                                         uncompressedDataBuffer.data() + uncompressedDataBuffer.size() - sizeof(adjustment),
                                                         sizeof(adjustment)));

                if(contentCRC != *assetBundleCRC) {
                    throw std::logic_error("the CRC-32 didn't come to the needed value after adjustment");
                }
            }
        }

        std::vector<unsigned char> compressedBody;

        if(dataCompression == UnityCompressionType::LZ4 || dataCompression == UnityCompressionType::LZ4HC) {
            /*
             * LZ4-compressed data needs to be chunked.
             */

            compressedBody.resize(uncompressedDataBuffer.size());

            auto dstPtr = compressedBody.data();
            auto srcPtr = uncompressedDataBuffer.data();
            auto srcEnd = uncompressedDataBuffer.data() + uncompressedDataBuffer.size();

            struct DataChunk {
                const unsigned char *uncompressedDataStart;
                size_t uncompressedDataSize;

                bool wasCompressed;

                unsigned char *compressedDataStart;
                size_t compressedDataSize;
            };

            std::vector<DataChunk> chunks;

            while(srcPtr < srcEnd) {
                auto block = std::min<size_t>(srcEnd - srcPtr, blockSize);

                chunks.emplace_back(DataChunk{
                    .uncompressedDataStart = srcPtr,
                    .uncompressedDataSize = block,
                    .wasCompressed = false,
                    .compressedDataStart = dstPtr,
                    .compressedDataSize = block
                });

                srcPtr += block;
                dstPtr += block;
            }

            std::for_each(std::execution::par_unseq, chunks.begin(), chunks.end(), [this](DataChunk &chunk) {
                chunk.wasCompressed = unityCompress(chunk.uncompressedDataStart, chunk.uncompressedDataSize, dataCompression,
                                                    chunk.compressedDataStart, chunk.compressedDataSize);
            });

            dstPtr = compressedBody.data();

            for(auto &chunk: chunks) {
                if(chunk.compressedDataStart != dstPtr) {
                    memmove(dstPtr, chunk.compressedDataStart, chunk.compressedDataSize);
                }

                auto &blockdef = directory.blocks.emplace_back();
                blockdef.compressedSize = chunk.compressedDataSize;
                blockdef.uncompressedSize = chunk.uncompressedDataSize;

                if(chunk.wasCompressed) {
                    blockdef.flags = static_cast<uint16_t>(dataCompression);
                } else {
                    blockdef.flags = static_cast<uint16_t>(UnityCompressionType::None);
                }

                dstPtr += chunk.compressedDataSize;
            }


            compressedBody.resize(dstPtr - compressedBody.data());
        } else {

            if(uncompressedDataBuffer.size() > std::numeric_limits<int32_t>::max())
                throw std::runtime_error("the uncompressed data is too long");

            size_t uncompressedLength = uncompressedDataBuffer.size();

            UnityCompressionType actualCompressionType;

            if(dataCompression == UnityCompressionType::None) {
                compressedBody = std::move(uncompressedDataBuffer);
                actualCompressionType = UnityCompressionType::None;

                compressedBody = std::move(uncompressedDataBuffer);
            } else {
                compressedBody.resize(uncompressedLength);

                size_t outputLength;
                if(unityCompress(uncompressedDataBuffer.data(), uncompressedDataBuffer.size(), dataCompression, compressedBody.data(), outputLength)) {

                    compressedBody.resize(outputLength);

                    actualCompressionType = dataCompression;
                } else {
                    compressedBody = std::move(uncompressedDataBuffer);
                    actualCompressionType = UnityCompressionType::None;
                }
            }

            if(compressedBody.size() > std::numeric_limits<int32_t>::max())
                throw std::runtime_error("the compressed data is too long");


            auto &blockdef = directory.blocks.emplace_back();
            blockdef.compressedSize = compressedBody.size();
            blockdef.uncompressedSize = uncompressedLength;
            blockdef.flags = static_cast<uint16_t>(actualCompressionType);
        }


        Stream uncompressedDirectory;
        directory.serialize(uncompressedDirectory);

        uint32_t uncompressedDirectoryLength = static_cast<uint32_t>(uncompressedDirectory.length());

        std::vector<unsigned char> compressedDirectory(uncompressedDirectory.length());
        size_t compressedDirectoryLength;
        uint32_t directoryFlags;
        if(unityCompress(uncompressedDirectory.data(), uncompressedDirectory.length(), directoryCompression, compressedDirectory.data(), compressedDirectoryLength)) {
            directoryFlags = static_cast<uint32_t>(directoryCompression) | BlocksAndDirectoryInfoCombined;
        } else {
            directoryFlags = static_cast<uint32_t>(UnityCompressionType::None) | BlocksAndDirectoryInfoCombined;
        }

        stream
            << static_cast<uint32_t>(compressedDirectoryLength)
            << static_cast<uint32_t>(uncompressedDirectoryLength)
            << static_cast<uint32_t>(directoryFlags);

        compressedDirectory.resize(compressedDirectoryLength);
        stream << compressedDirectory;
        stream << compressedBody;

        /*
         * Rewrite the file size with the correct value.
         */
        auto endPosition = stream.position();
        stream.setPosition(fileSizeOffset);
        fileSize = endPosition;
        stream << fileSize;
        stream.setPosition(endPosition);

    }

    /*
    * This calculates as 32-bit value that can be appended to a block of data
    * with CRC-32 value of 'originalCRC32' to make the CRC-32 of the combined
    * data to come out as 'desiredCRC32'.
    */
    uint32_t AssetBundleFile::calculateCRC32Adjustment(uint32_t originalCRC32, uint32_t desiredCRC32) {

        /*
        * The approach and the inverse polynomial are from
        * "Reversing CRC – Theory and Practice.", HU Berlin Public Report
        * S AR-PR-2006-05 by Martin Stigge, Henryk Plötz, Wolf Müller,
        * Jens-Peter Redlich
        */

        uint32_t adjustment = 0;

        originalCRC32 ^= UINT32_C(0xFFFFFFFF);
        desiredCRC32 ^= UINT32_C(0xFFFFFFFF);

        for(unsigned int bit = 0; bit < 32; bit++) {
            /*
            * Reduce modulo CRC-32 polynomial.
            */

            if(adjustment & 1) {
                adjustment = (adjustment >> 1) ^ UINT32_C(0xedb88320);
            } else {
                adjustment >>= 1;
            }

            /*
            * Add the inverse of the polynomial if the corresponding bit of the
            * desired CRC-32 is set.
            */

            if(desiredCRC32 & 1) {
                adjustment ^= UINT32_C(0x5B358FD3);
            }

            desiredCRC32 >>= 1;
        }

        return adjustment ^ originalCRC32;
    }

}
