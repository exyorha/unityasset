#include <UnityAsset/UnityTypeSerializer.h>
#include <UnityAsset/Environment/LoadedSerializedAsset.h>

namespace UnityAsset {

    UnityTypeSerializer::UnityTypeSerializer(Direction direction, Stream &stream, LoadedSerializedAsset *asset) : m_direction(direction), m_stream(stream),
        m_linkingAsset(asset) {

    }

    UnityTypeSerializer::~UnityTypeSerializer() = default;

    void UnityTypeSerializer::serializeValue(std::string &element) {
        if(isLinking())
            return;

        if(m_direction == Direction::Read) {
            int32_t length;
            m_stream >> length;
            element.resize(length);
            m_stream.readData(reinterpret_cast<unsigned char *>(element.data()), element.size());
        } else {
            m_stream << static_cast<int32_t>(element.size());
            m_stream.writeData(reinterpret_cast<const unsigned char *>(element.data()), element.size());
        }

        m_stream.alignPosition(4);
    }

    Downcastable *UnityTypeSerializer::resolvePointer(int32_t fileID, int64_t pathID) const {
        return m_linkingAsset->resolvePointer(fileID, pathID);
    }

    void UnityTypeSerializer::serializeValue(std::vector<bool> &element) {
        if(m_direction == Direction::Read) {
            int32_t length;
            m_stream >> length;
            element.resize(length);

            for(auto item: element) {
                bool value;
                serializeValue(value);
                item = value;
            }
        } else {
            if(!isLinking())
                m_stream << static_cast<int32_t>(element.size());

            for(auto item: element) {
                bool value = item;
                serializeValue(value);
            }
        }
        if(!isLinking())
            m_stream.alignPosition(4);
    }

}
