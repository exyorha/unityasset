 #include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>

namespace UnityAsset {
    InMemoryStreamBackingBuffer::InMemoryStreamBackingBuffer() = default;

    InMemoryStreamBackingBuffer::InMemoryStreamBackingBuffer(std::vector<unsigned char> &&data) : m_data(std::move(data)) {

    }

    InMemoryStreamBackingBuffer::~InMemoryStreamBackingBuffer() = default;

    void InMemoryStreamBackingBuffer::resize(size_t size) {
        m_data.resize(size);
    }

    const unsigned char *InMemoryStreamBackingBuffer::data() const {
        return m_data.data();
    }

    size_t InMemoryStreamBackingBuffer::size() const {
        return m_data.size();
    }

}
