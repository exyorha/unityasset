#include <UnityAsset/FileDescriptor.h>

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

#include <algorithm>

namespace UnityAsset {

    FileDescriptor::FileDescriptor() noexcept : m_fd(-1) {

    }

    FileDescriptor::FileDescriptor(int fd) noexcept : m_fd(fd) {

    }

    FileDescriptor::~FileDescriptor() {
        if(m_fd >= 0) {
#if defined(_WIN32)
            _close(m_fd);
#else
            close(m_fd);
#endif
        }
    }

    FileDescriptor::FileDescriptor(FileDescriptor &&other) noexcept : FileDescriptor() {
        swap(other);
    }

    FileDescriptor &FileDescriptor::operator =(FileDescriptor &&other) noexcept {
        if(this != &other)
            swap(other);

        return *this;
    }

    void FileDescriptor::swap(FileDescriptor &other) noexcept {
        std::swap(m_fd, other.m_fd);
    }

    void FileDescriptor::reset() noexcept {
        FileDescriptor discarded;
        discarded = std::move(*this);
    }

    [[nodiscard]] int FileDescriptor::release() noexcept {
        auto fd = m_fd;
        m_fd = -1;

        return fd;
    }
}
