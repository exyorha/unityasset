#ifndef UNITY_ASSET_STREAMS_STREAM_H
#define UNITY_ASSET_STREAMS_STREAM_H

#include <memory>
#include <string_view>
#include <bit>
#include <vector>

namespace UnityAsset {

    class StreamBackingBuffer;

    class Stream final {
    public:
        Stream();
        explicit Stream(const std::shared_ptr<StreamBackingBuffer> &backingBuffer,
                        size_t offset = 0,
                        size_t size = std::string_view::npos);
        ~Stream();

        enum class ByteOrder {
            MostSignificantFirst,
            LeastSignificantFirst
        };

        Stream(const Stream &other);
        Stream &operator =(const Stream &other);

        Stream(Stream &&other) noexcept;
        Stream &operator = (Stream &&other) noexcept;

        inline ByteOrder byteOrder() const {
            return m_byteOrder;
        }

        void setByteOrder(ByteOrder byteOrder);

        void readData(unsigned char *data, size_t size);
        size_t readSomeData(unsigned char *data, size_t size);
        void writeData(const unsigned char *data, size_t size);

        inline size_t position() const {
            return m_position;
        }

        void setPosition(size_t position);

        inline size_t length() const {
            return m_length;
        }

        Stream createView(size_t offset = 0, size_t size = std::string_view::npos) const;

        const unsigned char* data() const;

        inline Stream &operator <<(uint8_t value) {
            return byteOrderAwarePut(value, m_operationSet->putU8);
        }

        inline Stream &operator <<(uint16_t value) {
            return byteOrderAwarePut(value, m_operationSet->putU16);
        }

        inline Stream &operator <<(char16_t value) {
            return byteOrderAwarePut(static_cast<uint16_t>(value), m_operationSet->putU16);
        }

        inline Stream &operator <<(uint32_t value) {
            return byteOrderAwarePut(value, m_operationSet->putU32);
        }

        inline Stream &operator <<(uint64_t value) {
            return byteOrderAwarePut(value, m_operationSet->putU64);
        }

        Stream &operator >>(uint8_t &value) {
            return byteOrderAwareGet(value, m_operationSet->getU8);
        }

        Stream &operator >>(uint16_t &value) {
            return byteOrderAwareGet(value, m_operationSet->getU16);
        }

        Stream &operator >>(char16_t &value) {
            return byteOrderAwareGet(reinterpret_cast<uint16_t &>(value), m_operationSet->getU16);
        }

        Stream &operator >>(uint32_t &value) {
            return byteOrderAwareGet(value, m_operationSet->getU32);
        }

        Stream &operator >>(uint64_t &value) {
            return byteOrderAwareGet(value, m_operationSet->getU64);
        }

        inline Stream &operator <<(int8_t value) {
            return byteOrderAwarePut(value, m_operationSet->putS8);
        }

        inline Stream &operator <<(int16_t value) {
            return byteOrderAwarePut(value, m_operationSet->putS16);
        }

        inline Stream &operator <<(int32_t value) {
            return byteOrderAwarePut(value, m_operationSet->putS32);
        }

        inline Stream &operator <<(int64_t value)  {
            return byteOrderAwarePut(value, m_operationSet->putS64);
        }

        inline Stream &operator >>(int8_t &value) {
            return byteOrderAwareGet(value, m_operationSet->getS8);
        }

        inline Stream &operator >>(int16_t &value) {
            return byteOrderAwareGet(value, m_operationSet->getS16);
        }

        inline Stream &operator >>(int32_t &value) {
            return byteOrderAwareGet(value, m_operationSet->getS32);
        }

        inline Stream &operator >>(int64_t &value) {
            return byteOrderAwareGet(value, m_operationSet->getS64);
        }

        Stream &operator <<(float value);

        Stream &operator >>(float &value);

        Stream &operator <<(double value);

        Stream &operator >>(double &value);

        std::string_view readNullTerminatedString();
        void writeNullTerminatedString(const std::string_view &string);

        void alignPosition(size_t alignment);

    private:
        inline unsigned char *writableData() {
            return const_cast<unsigned char *>(data());
        }

        struct ByteOrderOperationSet {
            void (Stream::*putU8)(uint8_t value);
            void (Stream::*putU16)(uint16_t value);
            void (Stream::*putU32)(uint32_t value);
            void (Stream::*putU64)(uint64_t value);
            void (Stream::*getU8)(uint8_t &value);
            void (Stream::*getU16)(uint16_t &value);
            void (Stream::*getU32)(uint32_t &value);
            void (Stream::*getU64)(uint64_t &value);
            void (Stream::*putS8)(int8_t value);
            void (Stream::*putS16)(int16_t value);
            void (Stream::*putS32)(int32_t value);
            void (Stream::*putS64)(int64_t value);
            void (Stream::*getS8)(int8_t &value);
            void (Stream::*getS16)(int16_t &value);
            void (Stream::*getS32)(int32_t &value);
            void (Stream::*getS64)(int64_t &value);
        };

        static constexpr ByteOrder nativeByteOrder() {
            if constexpr(std::endian::native == std::endian::little) {
                return ByteOrder::LeastSignificantFirst;
            } else {
                return ByteOrder::MostSignificantFirst;
            }
        }

        template<typename T>
        inline Stream &byteOrderAwarePut(T value, void (Stream::*putter)(T value)) {
            (this->*putter)(value);
            return *this;
        }

        template<typename T>
        inline Stream &byteOrderAwareGet(T &value, void (Stream::*getter)(T &value)) {
            (this->*getter)(value);
            return *this;
        }

        template<typename T>
        void nativePut(T value);

        template<typename T>
        void nativeGet(T &value);

        template<typename T>
        void oppositePut(T value);

        template<typename T>
        void oppositeGet(T &value);

        inline int8_t swapByteOrder(int8_t value) { return value; }
        inline uint8_t swapByteOrder(uint8_t value) { return value; }

        inline int16_t swapByteOrder(int16_t value) { return static_cast<int16_t>(swapByteOrder(static_cast<uint16_t>(value))); }
        inline int32_t swapByteOrder(int32_t value) { return static_cast<int32_t>(swapByteOrder(static_cast<uint32_t>(value))); }
        inline int64_t swapByteOrder(int64_t value) { return static_cast<int64_t>(swapByteOrder(static_cast<uint64_t>(value))); }

#if defined(__cpp_lib_byteswap) && __cpp_lib_byteswap >= 202110L
        template<typename T>
        inline T swapByteOrder(T value) {
            return std::byteswap(value);
        }
#elif defined(_MSC_VER)
        inline uint16_t swapByteOrder(uint16_t value) {
            return _byteswap_ushort(value);
        }

        inline uint32_t swapByteOrder(uint32_t value) {
            return _byteswap_ulong(value);
        }

        inline uint64_t swapByteOrder(uint64_t value) {
            return _byteswap_uint64(value);
        }

#else
        inline uint16_t swapByteOrder(uint16_t value) {
            return __builtin_bswap16(value);
        }

        inline uint32_t swapByteOrder(uint32_t value) {
            return __builtin_bswap32(value);
        }

        inline uint64_t swapByteOrder(uint64_t value) {
            return __builtin_bswap64(value);
        }
#endif

        static const ByteOrderOperationSet m_nativeByteOrderOperationSet;
        static const ByteOrderOperationSet m_oppositeByteOrderOperationSet;

        ByteOrder m_byteOrder;
        const ByteOrderOperationSet *m_operationSet;
        std::shared_ptr<StreamBackingBuffer> m_backingBuffer;
        size_t m_offset;
        size_t m_length;
        size_t m_position;
    };


    template<size_t N>
    inline Stream &operator <<(Stream &stream, const char(&data)[N]) {
        stream.writeData(reinterpret_cast<const unsigned char *>(&data), N);
        return stream;
    }

    template<size_t N>
    inline Stream &operator >>(Stream &stream, char(&data)[N]) {
        stream.readData(reinterpret_cast<unsigned char *>(&data), N);
        return stream;
    }

    template<size_t N>
    inline Stream &operator <<(Stream &stream, const unsigned char(&data)[N]) {
        stream.writeData(data, N);
        return stream;
    }

    template<size_t N>
    inline Stream &operator >>(Stream &stream, unsigned char(&data)[N]) {
        stream.readData(data, N);
        return stream;
    }

    template<size_t N>
    inline Stream &operator <<(Stream &stream, const std::array<char, N> &data) {
        stream.writeData(reinterpret_cast<const unsigned char *>(data.data()), data.size());
        return stream;
    }

    template<size_t N>
    inline Stream &operator >>(Stream &stream, std::array<char, N> &data) {
        stream.readData(reinterpret_cast<unsigned char *>(data.data()), data.size());
        return stream;
    }

    template<size_t N>
    inline Stream &operator <<(Stream &stream, const std::array<unsigned char, N> &data) {
        stream.writeData(reinterpret_cast<const unsigned char *>(data.data()), data.size());
        return stream;
    }

    template<size_t N>
    inline Stream &operator >>(Stream &stream, std::array<unsigned char, N> &data) {
        stream.readData(reinterpret_cast<unsigned char *>(data.data()), data.size());
        return stream;
    }

    template<typename T>
    Stream &operator <<(Stream &stream, const std::vector<T> &vector) {
        for(const auto &entry: vector) {
            stream << entry;
        }

        return stream;
    }

    template<typename T>
    Stream &operator >>(Stream &stream, std::vector<T> &vector) {
        for(auto &entry: vector) {
            stream >> entry;
        }

        return stream;
    }

    template<>
    Stream &operator<<<unsigned char>(Stream &stream, const std::vector<unsigned char> &vector);

    template<>
    Stream &operator>><unsigned char>(Stream &stream, std::vector<unsigned char> &vector);

    template<>
    Stream &operator<<<char>(Stream &stream, const std::vector<char> &vector);

    template<>
    Stream &operator>><char>(Stream &stream, std::vector<char> &vector);

    Stream &operator <<(Stream &stream, bool value);
    Stream &operator >>(Stream &stream, bool &value);

}

#endif
