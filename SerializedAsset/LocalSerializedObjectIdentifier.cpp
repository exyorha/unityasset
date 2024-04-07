#include <UnityAsset/SerializedAsset/LocalSerializedObjectIdentifier.h>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    LocalSerializedObjectIdentifier::LocalSerializedObjectIdentifier(Stream &stream) {
        stream >> localSerializedFileIndex;
        stream.alignPosition(4);
        stream >> localIdentifierInFile;
    }

    LocalSerializedObjectIdentifier::~LocalSerializedObjectIdentifier() = default;

    LocalSerializedObjectIdentifier::LocalSerializedObjectIdentifier(LocalSerializedObjectIdentifier &&other) noexcept = default;

    LocalSerializedObjectIdentifier &LocalSerializedObjectIdentifier::operator =(LocalSerializedObjectIdentifier &&other) noexcept = default;

}
