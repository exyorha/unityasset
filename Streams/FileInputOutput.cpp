#include <UnityAsset/Streams/FileInputOutput.h>
#include <UnityAsset/Streams/InMemoryStreamBackingBuffer.h>
#include <UnityAsset/Streams/MappedFileStreamBackingBuffer.h>
#include <UnityAsset/Streams/Stream.h>

#include <UnityAsset/FileDescriptor.h>

#include <fstream>
#include <stdexcept>
#include <system_error>
#include <vector>

#if defined(_WIN32)
#include <UnityAsset/WindowsHandle.h>
#include <UnityAsset/WindowsError.h>
#else
#include <sys/stat.h>
#include <fcntl.h>
#endif

namespace UnityAsset {

#if defined(_WIN32)
    void readFile(
        const UnityAsset::WindowsHandle& fd,
        unsigned char* ptr,
        size_t offset,
        size_t size
    ) {
        OVERLAPPED overlapped;
        ZeroMemory(&overlapped, sizeof(overlapped));

        overlapped.Offset = static_cast<DWORD>(offset);
        if constexpr (sizeof(offset) > 4) {
            overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);
        }
        else {
            overlapped.OffsetHigh = 0;
        }

        if constexpr (sizeof(size) > sizeof(DWORD)) {
            if (size > std::numeric_limits<DWORD>::max())
                throw std::logic_error("attempted to read more than DWORD_MAX bytes at once");
        }

        DWORD bytesRead;
        auto result = ReadFile(fd, ptr, size, &bytesRead, &overlapped);
        if (!result)
            WindowsError::throwLastError();

        if (bytesRead != size)
            throw std::logic_error("short read");
    }

    std::shared_ptr<StreamBackingBuffer> readFile(
        const UnityAsset::WindowsHandle & fd,
        size_t offset,
        size_t size
    ) {

        LARGE_INTEGER fileSize;

        if (!GetFileSizeEx(fd, &fileSize))
            WindowsError::throwLastError();

        auto fileSizeT = static_cast<size_t>(fileSize.QuadPart);

        if (offset > fileSizeT)
            throw std::logic_error("readFile: offset is out of range");

        if (size == std::string::npos)
            size = fileSizeT - offset;
        else if (size + offset > fileSizeT)
            throw std::logic_error("readFile: size is out of range");

        if (size < 65536) {
            std::vector<unsigned char> data(size);

            readFile(fd, data.data(), offset, data.size());

            return std::make_shared<InMemoryStreamBackingBuffer>(std::move(data));
        }

        return std::make_shared<MappedFileStreamBackingBuffer>(fd, offset, size);
    }

    std::shared_ptr<StreamBackingBuffer> readFile(
        const std::filesystem::path& path,
        size_t offset,
        size_t size
    ) {
        auto rawHandle = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (rawHandle == INVALID_HANDLE_VALUE)
            WindowsError::throwLastError();

        UnityAsset::WindowsHandle handle(rawHandle);

        return readFile(handle, offset, size);
    }

#else
    void readFile(
        const UnityAsset::FileDescriptor &fd,
        unsigned char *ptr,
        size_t offset,
        size_t size
    ) {
        while(size > 0) {
            ssize_t result;

            do {
                result = pread(fd, ptr, size, offset);
            } while(result == -1 && errno == EINTR);

            if(result < 0)
                throw std::system_error(errno, std::generic_category());

            ptr += result;
            size -= result;
            offset += result;
        }
    }

    std::shared_ptr<StreamBackingBuffer> readFile(
        const UnityAsset::FileDescriptor &fd,
        size_t offset,
        size_t size
    ) {

        struct stat st;
        if(fstat(fd, &st) < 0)
            throw std::system_error(errno, std::generic_category());

        if(static_cast<off_t>(offset) > st.st_size)
            throw std::logic_error("readFile: offset is out of range");

        if(size == std::string::npos)
            size = st.st_size - offset;
        else if(static_cast<off_t>(size + offset) > st.st_size)
            throw std::logic_error("readFile: size is out of range");

        if(size < 65536) {
            std::vector<unsigned char> data(size);

            readFile(fd, data.data(), offset, data.size());

            return std::make_shared<InMemoryStreamBackingBuffer>(std::move(data));
        }

        return std::make_shared<MappedFileStreamBackingBuffer>(fd, offset, size);
    }

    std::shared_ptr<StreamBackingBuffer> readFile(
        const std::filesystem::path &path,
        size_t offset,
        size_t size
    ) {
        int rawfd = open(path.c_str(), O_RDONLY);
        if(rawfd < 0)
            throw std::system_error(errno, std::generic_category());

        UnityAsset::FileDescriptor fd(rawfd);

        return readFile(fd, offset, size);
    }
#endif

    void writeFile(const std::filesystem::path &path, const Stream &input) {
        std::ofstream stream;

        stream.exceptions(std::ios::badbit | std::ios::failbit | std::ios::eofbit);
        stream.open(path, std::ios::out | std::ios::trunc | std::ios::binary);

        stream.write(reinterpret_cast<const char *>(input.data()), input.length());
    }
}
