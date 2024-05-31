#ifndef UNITY_ASSET_STREAMS_FILE_INPUT_OUTPUT_H
#define UNITY_ASSET_STREAMS_FILE_INPUT_OUTPUT_H

#include <memory>
#include <filesystem>

namespace UnityAsset {
    class StreamBackingBuffer;
    class Stream;
    class FileDescriptor;
    class WindowsHandle;

#if defined(_WIN32)
    std::shared_ptr<StreamBackingBuffer> readFile(
        const WindowsHandle& fd,
        size_t offset = 0,
        size_t size = std::string::npos
    );

    void readFile(
        const WindowsHandle& fd,
        unsigned char* buffer,
        size_t offset,
        size_t size
    );
#else
    std::shared_ptr<StreamBackingBuffer> readFile(
        const FileDescriptor &fd,
        size_t offset = 0,
        size_t size = std::string::npos
    );

    void readFile(
        const FileDescriptor &fd,
        unsigned char *buffer,
        size_t offset,
        size_t size
    );
#endif

    std::shared_ptr<StreamBackingBuffer> readFile(
        const std::filesystem::path &path,
        size_t offset = 0,
        size_t size = std::string::npos
    );

    void writeFile(const std::filesystem::path &path, const Stream &stream);
}

#endif
