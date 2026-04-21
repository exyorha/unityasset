#ifndef UNITY_ASSET_ANIMATION_CURVE_BINDING_SET_H
#define UNITY_ASSET_ANIMATION_CURVE_BINDING_SET_H

#include <string_view>
#include <vector>

#include <UnityAsset/Animation/CurveBinding.h>

namespace UnityAsset {

    namespace UnityClasses {
        struct AnimationClip;
        struct Animator;
        struct GameObject;
        struct Transform;
    }

    class CurveBindingSet {
    public:
        CurveBindingSet(UnityClasses::AnimationClip *animation, UnityClasses::Animator *animator);
        ~CurveBindingSet();

        CurveBindingSet(const CurveBindingSet &other);
        CurveBindingSet &operator =(const CurveBindingSet &other);

        CurveBindingSet(CurveBindingSet &&other) noexcept;
        CurveBindingSet &operator =(CurveBindingSet &&other) noexcept;

        inline const std::vector<CurveBinding> &bindings() const {
            return m_bindings;
        }

        inline size_t totalCurves() const {
            return m_totalCurves;
        }

    private:
        static UnityClasses::GameObject *findChildGameObject(UnityClasses::GameObject *parent, const std::string_view &path);
        static UnityClasses::Transform *transformComponentOf(UnityClasses::GameObject *gameObject);

        std::vector<CurveBinding> m_bindings;
        size_t m_totalCurves;
    };

}

#endif
