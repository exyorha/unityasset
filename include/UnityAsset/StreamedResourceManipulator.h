#ifndef UNITY_ASSET_STREAMED_RESOURCE_MANIPULATOR_H
#define UNITY_ASSET_STREAMED_RESOURCE_MANIPULATOR_H

#include <cstring>
#include <vector>
#include <optional>
#include <list>

#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    class AssetBundleEntry;

    class StreamedResourceManipulator {
    public:
        explicit StreamedResourceManipulator(AssetBundleEntry &backingFile);
        ~StreamedResourceManipulator();

        StreamedResourceManipulator(const StreamedResourceManipulator &other) = delete;
        StreamedResourceManipulator &operator =(const StreamedResourceManipulator &other) = delete;

        void consumeRangeOfOriginalData(size_t offset, size_t size);
        size_t addNewData(std::vector<unsigned char> &&data);

        bool finalize();

        Stream getViewOfOriginalData(size_t offset, size_t size) const;

        inline bool isModified() const {
            return m_newStream.has_value();
        }

    private:
        std::list<std::pair<size_t, size_t>> m_unconsumedRanges;
        AssetBundleEntry &m_backingFile;
        std::optional<Stream> m_newStream;
    };

}

#endif
