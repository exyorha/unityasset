#include <UnityAsset/SerializedAsset/FileIdentifier.h>

#include <UnityAsset/Streams/Stream.h>

#include <cstring>

namespace UnityAsset {

    FileIdentifier::FileIdentifier() : type(0) {
        memset(guid.data(), 0, guid.size());
    }

    FileIdentifier::FileIdentifier(Stream &stream) {
        (void)stream.readNullTerminatedString();

        stream >> guid;
        stream >> type;

        pathName = stream.readNullTerminatedString();
    }

    FileIdentifier::~FileIdentifier() = default;

    FileIdentifier::FileIdentifier(FileIdentifier &&other) noexcept = default;

    FileIdentifier &FileIdentifier::operator =(FileIdentifier &&other) noexcept = default;

    void FileIdentifier::serialize(Stream &stream) const {
        stream.writeNullTerminatedString(std::string_view());
        stream << guid;
        stream << type;
        stream.writeNullTerminatedString(pathName);
    }
}

