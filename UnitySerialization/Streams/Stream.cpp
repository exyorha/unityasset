#include <UnityAsset/Streams/Stream.h>
#include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>

#include <cstring>

namespace UnityAsset {

    Stream::Stream() : Stream(std::make_shared<InMemoryStreamBackingBuffer>()) {

    }


    Stream::Stream(const std::shared_ptr<StreamBackingBuffer> &backingBuffer,
                   size_t offset,
                   size_t size) : m_backingBuffer(backingBuffer), m_offset(offset), m_length(size),
                   m_position(0) {

        setByteOrder(nativeByteOrder());

        auto storageSize = backingBuffer->size();

        if(offset > storageSize)
            throw std::logic_error("Stream: offset is out of range");

        if(size == std::string::npos) {
            m_length = storageSize - offset;
        } else if(m_length + offset > storageSize)
            throw std::logic_error("Stream: size is out of range");
    }

    Stream::~Stream() = default;

    Stream::Stream(const Stream &other) = default;

    Stream &Stream::operator =(const Stream &other) = default;

    Stream::Stream(Stream &&other) noexcept = default;

    Stream &Stream::operator = (Stream &&other) noexcept = default;

    void Stream::setByteOrder(ByteOrder byteOrder) {
        m_byteOrder = byteOrder;
        if(m_byteOrder == nativeByteOrder()) {
            m_operationSet = &m_nativeByteOrderOperationSet;
        } else {
            m_operationSet = &m_oppositeByteOrderOperationSet;
        }
    }


    template<typename T>
    void Stream::nativePut(T value) {
        writeData(reinterpret_cast<const unsigned char *>(&value), sizeof(value));
    }

    template<typename T>
    void Stream::nativeGet(T &value) {
        readData(reinterpret_cast<unsigned char *>(&value), sizeof(value));
    }

    template<typename T>
    void Stream::oppositePut(T value) {
        nativePut(swapByteOrder(value));
    }

    template<typename T>
    void Stream::oppositeGet(T &value) {
        nativeGet(value);
        value = swapByteOrder(value);
    }

    void Stream::readData(unsigned char *data, size_t size) {
        auto streamData = this->data();

        if(m_position + size > length()) {
            throw std::logic_error("attempted to read beyond the bounds of the stream");
        }

        memcpy(data, streamData + m_position, size);

        m_position += size;
    }

    size_t Stream::readSomeData(unsigned char *data, size_t size) {
        auto streamData = this->data();

        size = std::min<size_t>(size, m_length - m_position);

        memcpy(data, streamData + m_position, size);

        m_position += size;

        return size;
    }

    void Stream::writeData(const unsigned char *data, size_t size) {
        auto endPosition = m_position + size;
        if(endPosition > m_length) {
            m_length = endPosition;

            if(m_length + m_offset > m_backingBuffer->size())
                m_backingBuffer->resize(m_offset + m_length);
        }

        memcpy(writableData() + m_position, data, size);

        m_position += size;
    }

    void Stream::setPosition(size_t position) {
        m_position = position;

        if(m_position > m_length) {
            m_length = m_position;

            if(m_offset + m_length > m_backingBuffer->size()) {
                m_backingBuffer->resize(m_offset + m_length);
            }
        }
    }

    const unsigned char *Stream::data() const {
        return m_backingBuffer->data() + m_offset;
    }

    std::string_view Stream::readNullTerminatedString() {
        auto begin = reinterpret_cast<const char *>(data() + m_position);
        auto end = static_cast<const char *>(memchr(begin, 0, m_length - m_position));
        if(end == nullptr)
            throw std::runtime_error("the null terminator was not found");

        m_position += end - begin + 1;

        return std::string_view(begin, end);
    }

    void Stream::writeNullTerminatedString(const std::string_view &string) {
        writeData(reinterpret_cast<const unsigned char *>(string.data()), string.size());
        *this << static_cast<uint8_t>(0);
    }

    Stream Stream::createView(size_t offset, size_t size) const {
        if(offset > m_length)
            throw std::logic_error("Stream::createView: offset overruns the stream");

        if(size == std::string::npos)
            size = m_length - offset;
        else if(size + offset > m_length)
            throw std::logic_error("Stream::createView: size overruns the stream");

        return Stream(m_backingBuffer, m_offset + offset, size);
    }

    void Stream::alignPosition(size_t alignment) {
        setPosition((position() + (alignment - 1)) & ~(alignment - 1));
    }

    const Stream::ByteOrderOperationSet Stream::m_nativeByteOrderOperationSet{
        .putU8 = &Stream::nativePut<uint8_t>,
        .putU16 = &Stream::nativePut<uint16_t>,
        .putU32 = &Stream::nativePut<uint32_t>,
        .putU64 = &Stream::nativePut<uint64_t>,
        .getU8 = &Stream::nativeGet<uint8_t>,
        .getU16 = &Stream::nativeGet<uint16_t>,
        .getU32 = &Stream::nativeGet<uint32_t>,
        .getU64 = &Stream::nativeGet<uint64_t>,
        .putS8 = &Stream::nativePut<int8_t>,
        .putS16 = &Stream::nativePut<int16_t>,
        .putS32 = &Stream::nativePut<int32_t>,
        .putS64 = &Stream::nativePut<int64_t>,
        .getS8 = &Stream::nativeGet<int8_t>,
        .getS16 = &Stream::nativeGet<int16_t>,
        .getS32 = &Stream::nativeGet<int32_t>,
        .getS64 = &Stream::nativeGet<int64_t>
    };

    const Stream::ByteOrderOperationSet Stream::m_oppositeByteOrderOperationSet{
        .putU8 = &Stream::oppositePut<uint8_t>,
        .putU16 = &Stream::oppositePut<uint16_t>,
        .putU32 = &Stream::oppositePut<uint32_t>,
        .putU64 = &Stream::oppositePut<uint64_t>,
        .getU8 = &Stream::oppositeGet<uint8_t>,
        .getU16 = &Stream::oppositeGet<uint16_t>,
        .getU32 = &Stream::oppositeGet<uint32_t>,
        .getU64 = &Stream::oppositeGet<uint64_t>,
        .putS8 = &Stream::oppositePut<int8_t>,
        .putS16 = &Stream::oppositePut<int16_t>,
        .putS32 = &Stream::oppositePut<int32_t>,
        .putS64 = &Stream::oppositePut<int64_t>,
        .getS8 = &Stream::oppositeGet<int8_t>,
        .getS16 = &Stream::oppositeGet<int16_t>,
        .getS32 = &Stream::oppositeGet<int32_t>,
        .getS64 = &Stream::oppositeGet<int64_t>
    };

    template<>
    Stream &operator<<<unsigned char>(Stream &stream, const std::vector<unsigned char> &vector) {
        stream.writeData(vector.data(), vector.size());

        return stream;
    }

    template<>
    Stream &operator>><unsigned char>(Stream &stream, std::vector<unsigned char> &vector) {
        stream.readData(vector.data(), vector.size());

        return stream;
    }

    template<>
    Stream &operator<<<char>(Stream &stream, const std::vector<char> &vector) {
        stream.writeData(reinterpret_cast<const unsigned char *>(vector.data()), vector.size());

        return stream;
    }

    template<>
    Stream &operator>><char>(Stream &stream, std::vector<char> &vector) {
        stream.readData(reinterpret_cast<unsigned char *>(vector.data()), vector.size());

        return stream;
    }

    Stream &operator <<(Stream &stream, bool value) {
        return stream << static_cast<uint8_t>(value);
    }

    Stream &operator >>(Stream &stream, bool &value) {
        uint8_t byte;
        stream >> byte;

        if(byte != 0 && byte != 1)
            throw std::runtime_error("bad bool value");

        value = byte != 0;

        return stream;
    }


    Stream &Stream::operator <<(float value) {
        return *this << std::bit_cast<uint32_t>(value);
    }

    Stream &Stream::operator >>(float &value) {
        uint32_t ival;
        *this >> ival;

        value = std::bit_cast<float>(ival);

        return *this;
    }

    Stream &Stream::operator <<(double value) {
        return *this << std::bit_cast<uint64_t>(value);
    }

    Stream &Stream::operator >>(double &value) {
        uint64_t ival;
        *this >> ival;

        value = std::bit_cast<double>(ival);

        return *this;
    }
}
