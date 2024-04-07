#ifndef UNITY_ASSET_UNITY_TYPE_SERIALIZER_H
#define UNITY_ASSET_UNITY_TYPE_SERIALIZER_H

#include <cstdint>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    class Stream;

    class UnityTypeSerializer {
    protected:
        enum class Direction {
            Read,
            Write
        };

        UnityTypeSerializer(Direction direction, Stream &stream);
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
        static T deserializeObject(const Stream &stream, uint32_t flags) {
            Stream input(stream);

            UnityTypeSerializer serializer(Direction::Read, input);

            T result;

            serializer.serialize(result, flags);

            if(input.position() != stream.length()) {
                throw std::runtime_error("extra data after the expected end of the object");
            }

            return result;
        }

        template<typename T>
        static Stream serializeObject(T &object, uint32_t flags) {
            Stream output;

            UnityTypeSerializer serializer(Direction::Write, output);

            serializer.serialize(object, flags);

            return output;
        }

    private:
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
                m_stream << static_cast<int32_t>(element.size());

                for(auto &item: element) {
                    serializeValue(item);
                }
            }
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
            } else {
                m_stream << element;
            }
        }

        Direction m_direction;
        Stream &m_stream;
    };

}

#endif
