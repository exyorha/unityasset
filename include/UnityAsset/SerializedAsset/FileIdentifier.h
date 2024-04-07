#ifndef UNITY_ASSET_SERIALIZED_ASSET_FILE_IDENTIFIER_H
#define UNITY_ASSET_SERIALIZED_ASSET_FILE_IDENTIFIER_H

#include <array>
#include <cstdint>
#include <string>

namespace UnityAsset {

    class Stream;

    class FileIdentifier {
    public:
        explicit FileIdentifier(Stream &stream);
        ~FileIdentifier();

        FileIdentifier(const FileIdentifier &other) = delete;
        FileIdentifier &operator =(const FileIdentifier &other) = delete;

        FileIdentifier(FileIdentifier &&other) noexcept;
        FileIdentifier &operator =(FileIdentifier &&other) noexcept;

        std::array<uint8_t, 16> guid;
        uint32_t type;
        std::string pathName;

        void serialize(Stream &stream) const;
    };

}

#endif
