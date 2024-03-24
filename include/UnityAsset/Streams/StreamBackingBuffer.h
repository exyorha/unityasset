#ifndef UNITY_ASSET_STREAMS_STREAM_BACKING_BUFFER_H
#define UNITY_ASSET_STREAMS_STREAM_BACKING_BUFFER_H

#include <cstddef>

namespace UnityAsset {
    class StreamBackingBuffer {
    protected:
        StreamBackingBuffer();
        ~StreamBackingBuffer();

    public:
        StreamBackingBuffer(const StreamBackingBuffer &other) = delete;
        StreamBackingBuffer &operator =(const StreamBackingBuffer &other) = delete;

        virtual size_t size() const = 0;
        virtual void resize(size_t size) = 0;

        virtual const unsigned char *data() const = 0;

        inline unsigned char *data() {
            return const_cast<unsigned char *>(const_cast<const StreamBackingBuffer *>(this)->data());
        }
    };
}

#endif

