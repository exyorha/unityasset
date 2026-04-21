#include <UnityAsset/Animation/CurveAttributes.h>
#include <UnityAsset/Animation/CurveBinding.h>
#include <UnityAsset/UnityTypes.h>

namespace UnityAsset {

    CurveBinding::CurveBinding(size_t curveIndex, UnityClasses::GameObject *drivenObject, const UnityTypes::GenericBinding &genericBinding) :
        m_curveIndex(curveIndex),
        m_curveCount(1),
        m_drivenObject(drivenObject),
        m_attribute(genericBinding.attribute),
        m_typeID(genericBinding.typeID),
        m_customType(genericBinding.customType) {

        if(genericBinding.typeID == UnityClasses::Transform::ClassID && genericBinding.attribute == TransformCurveAttribute::Position) {
            m_curveCount = 3;
        } else if(genericBinding.typeID == UnityClasses::Transform::ClassID && genericBinding.attribute == TransformCurveAttribute::RotationQuaternion) {
            m_curveCount = 4;
        } else if(genericBinding.typeID == UnityClasses::Transform::ClassID && genericBinding.attribute == TransformCurveAttribute::Scale) {
            m_curveCount = 3;
        } else if(genericBinding.typeID == UnityClasses::Transform::ClassID && genericBinding.attribute == TransformCurveAttribute::RotationEuler) {
            m_curveCount = 3;
        }

        if(genericBinding.isPPtrCurve)
            throw std::runtime_error("PPtr curves are not supported yet");
    }

    CurveBinding::~CurveBinding() = default;

    CurveBinding::CurveBinding(const CurveBinding &other) = default;

    CurveBinding &CurveBinding::operator =(const CurveBinding &other) = default;

    CurveBinding::CurveBinding(CurveBinding &&other) noexcept = default;

    CurveBinding &CurveBinding::operator =(CurveBinding &&other) noexcept = default;

}
