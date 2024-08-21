#include <UnityAsset/FileContainer/AssetBundle/AssetBundleDirectoryEntry.h>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    Stream &operator <<(Stream &stream, const AssetBundleDirectoryEntry &directoryEntry) {
        stream << directoryEntry.fileOffset << directoryEntry.fileSize << directoryEntry.fileFlags;
        stream.writeNullTerminatedString(directoryEntry.path);
        return stream;
    }

    Stream &operator >>(Stream &stream, AssetBundleDirectoryEntry &directoryEntry) {
        stream >> directoryEntry.fileOffset >> directoryEntry.fileSize >> directoryEntry.fileFlags;
        directoryEntry.path = stream.readNullTerminatedString();
        return stream;
    }
}


