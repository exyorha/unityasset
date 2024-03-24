#include "UnityAsset/UnityCompression.h"
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h>
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleDirectory.h>

#include <UnityAsset/Streams/Stream.h>
#include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>

#include <zlib.h>

#include <limits>

namespace UnityAsset {

    AssetBundleFile::AssetBundleFile() : directoryCompression(UnityCompressionType::LZ4HC), dataCompression(UnityCompressionType::None), blockSize(128 * 1024),
        assetBundleCRC(0) {

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

        size_t likelyBlockSize = 0;
        for(const auto &block: directory.blocks) {
            if((block.flags & ~UINT16_C(0x3F)) != 0) {
                throw std::runtime_error("AssetBundleFile: unsupported block flags");
            }

            auto type = static_cast<UnityCompressionType>(block.flags & UINT16_C(0x3F));

            if(type != UnityCompressionType::None) {
                dataCompression = type;
                likelyBlockSize = std::max<size_t>(likelyBlockSize, block.uncompressedSize);
            }

            unityUncompress(compressedData, block.compressedSize, type, uncompressedData, block.uncompressedSize);
            compressedData += block.compressedSize;
            uncompressedData += block.uncompressedSize;
        }

        if(likelyBlockSize != 0) {
            likelyBlockSize = std::bit_ceil(likelyBlockSize);
            likelyBlockSize = std::max<size_t>(likelyBlockSize, 16384);
            blockSize = likelyBlockSize;
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
            totalUncompressedSize += entry.data().length();
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

            position += file.fileSize;
        }

        if(assetBundleCRC.has_value()) {
            auto contentCRC = static_cast<uint32_t>(crc32(UINT32_C(0), uncompressedDataBuffer.data(), uncompressedDataBuffer.size()));

            if(contentCRC != *assetBundleCRC) {
                printf("Content CRC needs to be corrected from %u to %u\n", contentCRC, *assetBundleCRC);
                throw std::runtime_error("implement CRC correction");
            }
        }

        std::vector<unsigned char> compressedBody;
        if(dataCompression == UnityCompressionType::None) {
            compressedBody = std::move(uncompressedDataBuffer);

            for(size_t remainingSize = compressedBody.size(), block; remainingSize != 0; remainingSize -= block) {
                block = std::min<size_t>(remainingSize, std::numeric_limits<int32_t>::max());

                auto &blockdef = directory.blocks.emplace_back();
                blockdef.compressedSize = block;
                blockdef.uncompressedSize = block;
                blockdef.flags = static_cast<uint16_t>(UnityCompressionType::None);
            }
        } else {
            compressedBody.resize(uncompressedDataBuffer.size());

            auto dstPtr = compressedBody.data();
            auto srcPtr = uncompressedDataBuffer.data();
            auto srcEnd = uncompressedDataBuffer.data() + uncompressedDataBuffer.size();

            while(srcPtr < srcEnd) {
                auto block = std::min<size_t>(srcEnd - srcPtr, blockSize);

                size_t outputLengthWritten;
                auto blockWasCompressed = unityCompress(srcPtr, block, dataCompression, dstPtr, outputLengthWritten);

                auto &blockdef = directory.blocks.emplace_back();
                blockdef.compressedSize = outputLengthWritten;
                blockdef.uncompressedSize = block;

                if(blockWasCompressed) {
                    blockdef.flags = static_cast<uint16_t>(dataCompression);
                } else {
                    blockdef.flags = static_cast<uint16_t>(UnityCompressionType::None);
                }

                srcPtr += block;
                dstPtr += outputLengthWritten;
            }

            compressedBody.resize(dstPtr - compressedBody.data());
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
}
