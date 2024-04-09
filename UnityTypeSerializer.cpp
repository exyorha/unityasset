#include <UnityAsset/UnityTypeSerializer.h>

namespace UnityAsset {

    UnityTypeSerializer::UnityTypeSerializer(Direction direction, Stream &stream) : m_direction(direction), m_stream(stream) {

    }

    UnityTypeSerializer::~UnityTypeSerializer() = default;

    void UnityTypeSerializer::serializeValue(std::string &element) {
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
}
