#ifndef UNITY_ASSET_SERIALIZED_ASSET_LOCAL_SERIALIZED_OBJECT_IDENTIFIER_H
#define UNITY_ASSET_SERIALIZED_ASSET_LOCAL_SERIALIZED_OBJECT_IDENTIFIER_H

#include <cstdint>

namespace UnityAsset {

    class Stream;

    class LocalSerializedObjectIdentifier {
    public:
        explicit LocalSerializedObjectIdentifier(Stream &stream);
        ~LocalSerializedObjectIdentifier();

        LocalSerializedObjectIdentifier(const LocalSerializedObjectIdentifier &other) = delete;
        LocalSerializedObjectIdentifier &operator =(const LocalSerializedObjectIdentifier &other) = delete;

        LocalSerializedObjectIdentifier(LocalSerializedObjectIdentifier &&other) noexcept;
        LocalSerializedObjectIdentifier &operator =(LocalSerializedObjectIdentifier &&other) noexcept;

        int32_t localSerializedFileIndex;
        int64_t localIdentifierInFile;
    };

}

#endif
