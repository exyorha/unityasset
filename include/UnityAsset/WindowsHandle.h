#ifndef UNITY_ASSET_WINDOWS_HANDLE_H
#define UNITY_ASSET_WINDOWS_HANDLE_H

#include <windows.h>

namespace UnityAsset {

    class WindowsHandle {
    public:
        WindowsHandle() noexcept;
        explicit WindowsHandle(HANDLE fd) noexcept;
        ~WindowsHandle();

        WindowsHandle(const WindowsHandle& other) = delete;
        WindowsHandle& operator =(const WindowsHandle& other) = delete;

        WindowsHandle(WindowsHandle&& other) noexcept;
        WindowsHandle& operator =(WindowsHandle&& other) noexcept;

        inline operator HANDLE() const noexcept {
            return m_fd;
        }

        inline HANDLE get() const noexcept {
            return m_fd;
        }

        void swap(WindowsHandle& other) noexcept;
        void reset() noexcept;
        [[nodiscard]] HANDLE release() noexcept;

    private:
        HANDLE m_fd;
    };
}

#endif

