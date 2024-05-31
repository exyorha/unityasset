#ifndef UNITY_ASSET_UNITY_TYPE_SERIALIZER_H
#define UNITY_ASSET_UNITY_TYPE_SERIALIZER_H

#include <cstdint>

#include <UnityAsset/Streams/Stream.h>
#include <UnityAsset/SerializedAsset/Downcastable.h>
#include <UnityAsset/SerializedAsset/ExternalAssetData.h>

#include <type_traits>
#include <optional>

namespace UnityAsset {

    class Stream;
    template<typename T> class ObjectPointer;

    class AssetLinker;

    class UnityTypeSerializer {
    protected:
        enum class Direction {
            Read,
            Write,
            Linking
        };

        UnityTypeSerializer(Direction direction, Stream &stream, AssetLinker *asset = nullptr);
        ~UnityTypeSerializer();

    public:
        UnityTypeSerializer(const UnityTypeSerializer &other) = delete;
        UnityTypeSerializer &operator =(const UnityTypeSerializer &other) = delete;

        template<typename T>
        inline void serialize(T &element, uint32_t flags) {
            serializeValue(element);

            if(flags & 0x4000) {
                m_stream.alignPosition(4);
            }
        }

        template<typename T>
        static void deserializeObject(const Stream &stream, uint32_t flags, T &result) {
            Stream input(stream);

            UnityTypeSerializer serializer(Direction::Read, input);

            serializer.serialize(result, flags);

            if(input.position() != stream.length()) {
                throw std::runtime_error("extra data after the expected end of the object");
            }
        }

        template<typename T>
        static void serializeObject(T &object, uint32_t flags, Stream &output) {
            UnityTypeSerializer serializer(Direction::Write, output);

            serializer.serialize(object, flags);
        }

        template<typename T>
        static inline void linkObject(AssetLinker *asset, T &object, uint32_t flags) {
            Stream stream;
            UnityTypeSerializer serializer(Direction::Linking, stream, asset);

            serializer.serialize(object, flags);
        }

        template<typename RT, typename T>
        inline auto bindPointer(T &pointer) const -> typename std::enable_if<std::is_base_of_v<ObjectPointer<RT>, T>>::type {
            if(isLinking()) {
                pointer.link(object_cast<RT>(resolvePointer(pointer.m_FileID, pointer.m_PathID)));
            }
        }

        template<typename T>
        inline auto bindExternalAssetData(T &reference) const ->
            typename std::enable_if<std::is_base_of_v<ExternalAssetData, T>>::type {

            if(isLinking()) {
                reference.link(resolveExternalAssetData(reference.offset, reference.size, reference.path));
            }
        }

    private:
        Downcastable *resolvePointer(int32_t fileID, int64_t pathID) const;
        std::optional<Stream> resolveExternalAssetData(uint32_t offset, uint32_t size, const std::string &path) const;

        inline bool isLinking() const {
            return m_direction == Direction::Linking;
        }

        template<typename T>
        inline auto serializeValue(T &element) -> typename std::enable_if<std::is_compound<T>::value>::type {
            element.serialize(*this);
        }

        template<typename T>
        void serializeValue(std::vector<T> &element) {
            if(m_direction == Direction::Read) {
                int32_t length;
                m_stream >> length;
                element.resize(length);

                for(auto &item: element) {
                    serializeValue(item);
                }
            } else {
                if(!isLinking())
                    m_stream << static_cast<int32_t>(element.size());

                for(auto &item: element) {
                    serializeValue(item);
                }
            }
            if(!isLinking())
                m_stream.alignPosition(4);
        }

        void serializeValue(std::vector<bool> &element);

        template<typename K, typename V>
        inline void serializeValue(std::pair<K, V> &element) {
            serializeValue(element.first);
            serializeValue(element.second);
        }

        template<typename T, size_t Len>
        void serializeValue(std::array<T, Len> &element) {
            for(auto &item: element) {
                serializeValue(item);
            }
        }

        void serializeValue(std::string &element);

        template<typename T>
        auto serializeValue(T &element) -> typename std::enable_if<!std::is_compound<T>::value>::type {
            if(m_direction == Direction::Read) {
                m_stream >> element;
            } else if(m_direction == Direction::Write) {
                m_stream << element;
            }
        }

        Direction m_direction;
        Stream &m_stream;
        AssetLinker *m_linkingAsset;
    };

}

#endif
