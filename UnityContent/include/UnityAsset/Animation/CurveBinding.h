#ifndef UNITY_ASSET_ANIMATION_CURVE_BINDING_H
#define UNITY_ASSET_ANIMATION_CURVE_BINDING_H

#include <cstddef>
#include <cstdint>

namespace UnityAsset {

    namespace UnityTypes {
        struct GenericBinding;
    }

    namespace UnityClasses {
        struct GameObject;
    }

    class CurveBinding {
    public:
        CurveBinding(size_t curveIndex, UnityClasses::GameObject *drivenObject, const UnityTypes::GenericBinding &genericBinding);
        ~CurveBinding();

        CurveBinding(const CurveBinding &other);
        CurveBinding &operator =(const CurveBinding &other);

        CurveBinding(CurveBinding &&other) noexcept;
        CurveBinding &operator =(CurveBinding &&other) noexcept;

        inline size_t curveIndex() const {
            return m_curveIndex;
        }

        inline size_t curveCount() const {
            return m_curveCount;
        }

        inline UnityClasses::GameObject *drivenObject() const {
            return m_drivenObject;
        }

        inline uint32_t pathID() const {
            return m_pathID;
        }

        inline uint32_t attribute() const {
            return m_attribute;
        }

        inline uint32_t typeID() const {
            return m_typeID;
        }

        inline uint32_t customType() const {
            return m_customType;
        }

    private:
        size_t m_curveIndex;
        size_t m_curveCount;
        UnityClasses::GameObject *m_drivenObject;
        uint32_t m_pathID;
        uint32_t m_attribute;
        uint32_t m_typeID;
        uint32_t m_customType;
    };
}

#endif
