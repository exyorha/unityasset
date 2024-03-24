#ifndef UNITY_ASSET_STREAMS_MAPPED_FILE_STREAM_BACKING_BUFFER_H
#define UNITY_ASSET_STREAMS_MAPPED_FILE_STREAM_BACKING_BUFFER_H

#include <UnityAsset/Streams/StreamBackingBuffer.h>

#if defined(_WIN32)
#include <UnityAsset/WindowsHandle.h>
#include <cstdint>
#else
#include <sys/mman.h>
#endif

namespace UnityAsset {

    class MappedFileStreamBackingBuffer final : public StreamBackingBuffer {
    public:
#if defined(_WIN32)
        MappedFileStreamBackingBuffer(const WindowsHandle &fd, uint64_t offset, size_t size);
#else
        MappedFileStreamBackingBuffer(int fd, off_t offset, size_t size);
#endif

        ~MappedFileStreamBackingBuffer();

        size_t size() const override;
        void resize(size_t size) override;

        const unsigned char *data() const override;

    private:
#if defined(_WIN32)
        WindowsHandle m_mapping;
#endif
        void *m_data;
#if !defined(_WIN32)
        size_t m_size;
#endif
        const unsigned char *m_dataForAccess;
        size_t m_sizeForAccess;

        static unsigned long queryPageSize();
        static const unsigned long m_pageSize;
    };
}

#endif
