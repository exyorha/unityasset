#include <UnityAsset/WindowsHandle.h>

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

#include <algorithm>

namespace UnityAsset {

    WindowsHandle::WindowsHandle() noexcept : m_fd(nullptr) {

    }

    WindowsHandle::WindowsHandle(HANDLE fd) noexcept : m_fd(fd) {

    }

    WindowsHandle::~WindowsHandle() {
        if (m_fd != nullptr) {
            CloseHandle(m_fd);
        }
    }

    WindowsHandle::WindowsHandle(WindowsHandle&& other) noexcept : WindowsHandle() {
        swap(other);
    }

    WindowsHandle& WindowsHandle::operator =(WindowsHandle&& other) noexcept {
        if (this != &other)
            swap(other);

        return *this;
    }

    void WindowsHandle::swap(WindowsHandle& other) noexcept {
        std::swap(m_fd, other.m_fd);
    }

    void WindowsHandle::reset() noexcept {
        WindowsHandle discarded;
        discarded = std::move(*this);
    }

    [[nodiscard]] HANDLE WindowsHandle::release() noexcept {
        auto fd = m_fd;
        m_fd = nullptr;

        return fd;
    }
}
