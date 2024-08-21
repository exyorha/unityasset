#ifndef UNITY_ASSET_FILE_DESCRIPTOR_H
#define UNITY_ASSET_FILE_DESCRIPTOR_H

namespace UnityAsset {

    class FileDescriptor {
    public:
        FileDescriptor() noexcept;
        explicit FileDescriptor(int fd) noexcept;
        ~FileDescriptor();

        FileDescriptor(const FileDescriptor &other) = delete;
        FileDescriptor &operator =(const FileDescriptor &other) = delete;

        FileDescriptor(FileDescriptor &&other) noexcept;
        FileDescriptor &operator =(FileDescriptor &&other) noexcept;

        inline operator int() const noexcept {
            return m_fd;
        }

        inline int get() const noexcept {
            return m_fd;
        }

        inline explicit operator bool() const noexcept {
            return m_fd >= 0;
        }

        void swap(FileDescriptor &other) noexcept;
        void reset() noexcept;
        [[nodiscard]] int release() noexcept;

    private:
        int m_fd;
    };
}

#endif

