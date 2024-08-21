#ifndef UNITY_ASSET_STREAMS_IN_MEMORY_STREAM_BACKING_BUFFER_H
#define UNITY_ASSET_STREAMS_IN_MEMORY_STREAM_BACKING_BUFFER_H

#include <UnityAsset/Streams/StreamBackingBuffer.h>

#include <vector>

namespace UnityAsset {

    class InMemoryStreamBackingBuffer final : public StreamBackingBuffer {
    public:
        InMemoryStreamBackingBuffer();
        explicit InMemoryStreamBackingBuffer(std::vector<unsigned char> &&data);
        ~InMemoryStreamBackingBuffer();

        size_t size() const override;
        void resize(size_t size) override;

        const unsigned char *data() const override;

    private:
        std::vector<unsigned char> m_data;

    };

}

#endif

