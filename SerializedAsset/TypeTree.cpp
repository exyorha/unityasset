#include <UnityAsset/SerializedAsset/TypeTree.h>
#include <UnityAsset/Streams/Stream.h>

namespace UnityAsset {

    TypeTree::TypeTree(Stream &stream, bool isRefType) {
        int32_t numberOfNodes;
        int32_t stringBufferSize;

        stream >> numberOfNodes >> stringBufferSize;

        m_Nodes.reserve(numberOfNodes);
        for(int32_t index = 0; index < numberOfNodes; index++) {
            m_Nodes.emplace_back(stream);
        }

        m_StringBuffer = stream.createView(stream.position(), stringBufferSize);
        stream.setPosition(stream.position() + stringBufferSize);

        if(isRefType) {
            auto &ref = referenceTypeData.emplace();

            ref.m_KlassName = stream.readNullTerminatedString();
            ref.m_NameSpace = stream.readNullTerminatedString();
            ref.m_AsmName = stream.readNullTerminatedString();
        } else {

            auto &deps = m_TypeDependencies.emplace();

            int32_t numberOfDependencies;
            stream >> numberOfDependencies;

            deps.resize(numberOfDependencies);

            stream >> deps;
        }
    }

    TypeTree::~TypeTree() = default;

    TypeTree::TypeTree(TypeTree &&other) noexcept = default;

    TypeTree &TypeTree::operator =(TypeTree &&other) noexcept = default;

    void TypeTree::serialize(Stream &stream) const {
        stream
            << static_cast<int32_t>(m_Nodes.size())
            << static_cast<int32_t>(m_StringBuffer.length());

        for(const auto &node: m_Nodes) {
            node.serialize(stream);
        }

        stream.writeData(m_StringBuffer.data(), m_StringBuffer.length());

        if(referenceTypeData.has_value()) {
            const auto &ref = *referenceTypeData;

            stream.writeNullTerminatedString(ref.m_KlassName);
            stream.writeNullTerminatedString(ref.m_NameSpace);
            stream.writeNullTerminatedString(ref.m_AsmName);
        }

        if(m_TypeDependencies.has_value()) {
            const auto &deps = *m_TypeDependencies;

            stream << static_cast<int32_t>(deps.size());
            stream << deps;
        }
    }
}

