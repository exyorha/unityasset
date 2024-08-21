#include <UnityAsset/ShaderBlob.h>

#include <list>

#include <lz4.h>
#include <lz4hc.h>

namespace UnityAsset {

    ShaderBlob::ShaderBlob(const std::vector<uint32_t> &compressedLengths, const std::vector<uint32_t> &decompressedLengths,
                           const std::vector<uint32_t> &offsets, const std::vector<unsigned char> &compressedBlob) {

        if(compressedLengths.size() != decompressedLengths.size() || compressedLengths.size() != offsets.size()) {
            throw std::runtime_error("mismatched number of segments");
        }

        if(compressedLengths.empty()) {
            // No segments = no entries

            return;
        }

        size_t numberOfSegments = compressedLengths.size();
        std::vector<Stream> uncompressedSegments;
        uncompressedSegments.reserve(numberOfSegments);

        for(size_t segmentIndex = 0; segmentIndex < numberOfSegments; segmentIndex++) {
            UnityAsset::Stream segmentStream;

            segmentStream.setPosition(decompressedLengths[segmentIndex]);
            segmentStream.setPosition(0);

            auto result = LZ4_decompress_safe(
                reinterpret_cast<const char *>(compressedBlob.data()) + offsets[segmentIndex],
                const_cast<char *>(reinterpret_cast<const char *>(segmentStream.data())),
                compressedLengths[segmentIndex],
                segmentStream.length());

            if(result != static_cast<int>(segmentStream.length())) {
                throw std::runtime_error("unexpected length of the compressed shader data");
            }

            segmentStream.setByteOrder(Stream::ByteOrder::LeastSignificantFirst);

            uncompressedSegments.emplace_back(std::move(segmentStream));
        }

        auto &headerStream = uncompressedSegments.front();
        int32_t numberOfEntries;
        headerStream >> numberOfEntries;

        entries.reserve(numberOfEntries);

        for(int32_t index = 0; index <numberOfEntries; index++) {
            int32_t offset;
            int32_t length;
            int32_t segment;

            headerStream >> offset >> length >> segment;

            entries.emplace_back(uncompressedSegments.at(segment).createView(offset, length));
        }
    }

    ShaderBlob::~ShaderBlob() = default;

    ShaderBlob::ShaderBlob(ShaderBlob &&other) noexcept = default;

    ShaderBlob &ShaderBlob::operator =(ShaderBlob &&other) noexcept = default;

    void ShaderBlob::serialize(
            std::vector<uint32_t> &compressedLengths,
            std::vector<uint32_t> &decompressedLengths,
            std::vector<uint32_t> &offsets,
            std::vector<unsigned char> &compressedBlob) const {

        if(entries.empty()) {
            /*
             * No entries -> no segments.
             */
        }

        std::list<Stream> uncompressedSegments;

        auto &headerStream = uncompressedSegments.emplace_back();

        size_t offset = 4 + 12 * entries.size();
        size_t segmentIndex = 0;

        headerStream.setByteOrder(Stream::ByteOrder::LeastSignificantFirst);

        headerStream << static_cast<uint32_t>(entries.size());

        for(const auto &entry: entries) {
            auto headerPosition = headerStream.position();

            if(((offset + 15) & ~15) + entry.length() > 16 * 1024 * 1024) {
                segmentIndex++;
                offset = 0;
                uncompressedSegments.emplace_back().setByteOrder(Stream::ByteOrder::LeastSignificantFirst);
            }


            auto writingToStream = uncompressedSegments.end();
            --writingToStream;

            writingToStream->setPosition(offset);
            writingToStream->alignPosition(16);

            auto entryPosition = writingToStream->position();
            writingToStream->writeData(entry.data(), entry.length());

            offset = writingToStream->position();

            headerStream.setPosition(headerPosition);

            headerStream << static_cast<int32_t>(entryPosition);
            headerStream << static_cast<int32_t>(entry.length());
            headerStream << static_cast<int32_t>(segmentIndex);
        }

        for(const auto &segment: uncompressedSegments) {
            auto bound = LZ4_compressBound(segment.length());
            auto position = compressedBlob.size();
            compressedBlob.resize(position + bound);

            auto compressedBytes = LZ4_compress_HC(
                reinterpret_cast<const char *>(segment.data()),
                reinterpret_cast<char *>(compressedBlob.data()) + position,
                segment.length(),
                bound,
                LZ4HC_CLEVEL_MAX);

            if(compressedBytes == 0) {
                throw std::runtime_error("failed to compress");
            }

            compressedBlob.resize(position + compressedBytes);

            compressedLengths.emplace_back(compressedBytes);
            decompressedLengths.emplace_back(segment.length());
            offsets.emplace_back(position);
        }
    }
}
