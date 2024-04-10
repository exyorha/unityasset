#include "UnityAsset/Streams/InMemoryStreamBackingBuffer.h"
#include <UnityAsset/StreamedResourceManipulator.h>
#include <UnityAsset/FileContainer/AssetBundle/AssetBundleEntry.h>

namespace UnityAsset {
    StreamedResourceManipulator::StreamedResourceManipulator(AssetBundleEntry &backingFile) : m_backingFile(backingFile) {
        if(backingFile.data().length() != 0) {
            m_unconsumedRanges.emplace_back(0, backingFile.data().length());
        }
    }

    StreamedResourceManipulator::~StreamedResourceManipulator() = default;

    void StreamedResourceManipulator::consumeRangeOfOriginalData(size_t offset, size_t size) {
        size_t endOffset = offset + size;

        offset = offset & ~1;
        endOffset = (endOffset + 1) & ~1;

        for(auto it = m_unconsumedRanges.begin(); it != m_unconsumedRanges.end(); it++) {
            auto &range = *it;

            if(range.first > offset) {
                break;
            }

            if(range.first <= offset && range.second >= endOffset) {
                if(range.first < offset) {
                    m_unconsumedRanges.emplace(it, range.first, offset);
                }

                range.first = endOffset;
                if(range.second == endOffset) {
                    it = m_unconsumedRanges.erase(it);
                }

                return;
            }
        }

        throw std::runtime_error("the requested byte range is already consumed");
    }

    size_t StreamedResourceManipulator::addNewData(std::vector<unsigned char> &&data) {
        if(!m_newStream.has_value()) {
            m_newStream.emplace(std::make_shared<InMemoryStreamBackingBuffer>(std::move(data)));

            return 0;
        } else {
            auto &stream = *m_newStream;

            auto offset = stream.length();
            stream.writeData(data.data(), data.size());

            stream.alignPosition(2);

            return offset;
        }
    }

    bool StreamedResourceManipulator::finalize() {
        if(!m_newStream.has_value())
            return false;

        if(!m_unconsumedRanges.empty()) {
            throw std::runtime_error("unconsumed ranges remain during StreamedResourceManipulator finalization");
        }

        m_backingFile.replace(std::move(*m_newStream));

        m_newStream.reset();

        if(m_backingFile.data().length() != 0) {
            m_unconsumedRanges.emplace_back(0, m_backingFile.data().length());
        }

        return true;
    }


    Stream StreamedResourceManipulator::getViewOfOriginalData(size_t offset, size_t size) const {
        return m_backingFile.data().createView(offset, size);
    }

}
