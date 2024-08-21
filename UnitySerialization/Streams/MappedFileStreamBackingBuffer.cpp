#include <UnityAsset/Streams/MappedFileStreamBackingBuffer.h>

#if defined(_WIN32)
#include <UnityAsset/WindowsError.h>
#else
#include <unistd.h>
#endif

#include <stdexcept>
#include <system_error>

namespace UnityAsset {
    const unsigned long MappedFileStreamBackingBuffer::m_pageSize = queryPageSize();

#if defined(_WIN32)
    MappedFileStreamBackingBuffer::MappedFileStreamBackingBuffer(const WindowsHandle& fd, uint64_t offset, size_t size) {
        if (size == 0) {
            m_data = nullptr;
            m_dataForAccess = nullptr;
            m_sizeForAccess = 0;
        }
        else {
            m_sizeForAccess = size;

            auto adjustedOffset = offset & ~static_cast<uint64_t>(m_pageSize - 1);
            auto extraSize = offset - adjustedOffset;
            size += extraSize;

            auto rawMapping = CreateFileMapping(fd, nullptr, PAGE_READONLY, 0, 0, nullptr);
            if (!rawMapping)
                WindowsError::throwLastError();

            m_mapping = WindowsHandle(rawMapping);

            DWORD offsetLow = static_cast<DWORD>(adjustedOffset);
            DWORD offsetHigh = static_cast<DWORD>(adjustedOffset >> 32);

            m_data = MapViewOfFile(
                m_mapping,
                FILE_MAP_READ,
                offsetHigh,
                offsetLow,
                size
            );
            if (m_data == nullptr)
                WindowsError::throwLastError();

            m_dataForAccess = static_cast<unsigned char*>(m_data) + extraSize;
        }

    }

    MappedFileStreamBackingBuffer::~MappedFileStreamBackingBuffer() {
        if (m_data != nullptr) {
            UnmapViewOfFile(m_data);
        }
    }

    unsigned long MappedFileStreamBackingBuffer::queryPageSize() {
        SYSTEM_INFO info;
        GetSystemInfo(&info);

        return info.dwAllocationGranularity;
    }

#else
    MappedFileStreamBackingBuffer::MappedFileStreamBackingBuffer(int fd, off_t offset, size_t size) {
        if(size == 0) {
            m_data = nullptr;
            m_size = 0;
            m_dataForAccess = nullptr;
            m_sizeForAccess = 0;
        } else {
            m_sizeForAccess = size;

            auto adjustedOffset = offset & ~(m_pageSize - 1);
            auto extraSize = offset - adjustedOffset;
            size += extraSize;

            m_size = size;
            m_data = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, adjustedOffset);
            if(m_data == MAP_FAILED)
                throw std::system_error(errno, std::generic_category());

            m_dataForAccess = static_cast<unsigned char *>(m_data) + extraSize;
        }
    }

    MappedFileStreamBackingBuffer::~MappedFileStreamBackingBuffer() {
        if(m_data != nullptr) {
            munmap(m_data, m_size);
        }
    }

    unsigned long MappedFileStreamBackingBuffer::queryPageSize() {
        auto pageSize = sysconf(_SC_PAGESIZE);
        if (pageSize < 0)
            throw std::system_error(errno, std::generic_category());

        if (pageSize & (pageSize - 1))
            throw std::logic_error("the page size is not a power of two");

        return static_cast<unsigned long>(pageSize);
    }
#endif


    size_t MappedFileStreamBackingBuffer::size() const {
        return m_sizeForAccess;
    }

    void MappedFileStreamBackingBuffer::resize(size_t size) {
        (void)size;

        throw std::logic_error("MappedFileStreamBackingBuffer::resize: not resizable");
    }

    const unsigned char *MappedFileStreamBackingBuffer::data() const {
        return m_dataForAccess;
    }
}
