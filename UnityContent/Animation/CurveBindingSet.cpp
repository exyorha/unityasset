#include <UnityAsset/Animation/CurveBindingSet.h>
#include <UnityAsset/Animation/CurveAttributes.h>
#include <UnityAsset/UnityTypes.h>

#include <sstream>
#include <stdexcept>

namespace UnityAsset {

    CurveBindingSet::CurveBindingSet(UnityClasses::AnimationClip *animation, UnityClasses::Animator *animator) {

        std::unordered_map<uint32_t, std::string_view> tos;
        tos.reserve(animator->m_Avatar->m_TOS.size());

        for(const auto &tosEntry: animator->m_Avatar->m_TOS) {
            tos.emplace(tosEntry.first, tosEntry.second);
        }

        size_t curveIndex = 0;

        for(const auto &binding: animation->m_ClipBindingConstant.genericBindings) {
            auto resolvedPath = tos.find(binding.path);
            if(resolvedPath == tos.end()) {
                std::stringstream err;
                err << "animation binding is not in the avatar's TOS: " << binding.path;
                throw std::runtime_error(err.str());
            }

            auto go = findChildGameObject(animator->UnityTypes::Animator::m_GameObject, resolvedPath->second);

            auto &outBinding = m_bindings.emplace_back(curveIndex, go, binding);

            curveIndex += outBinding.curveCount();
        }

        m_totalCurves = curveIndex;

    }

    CurveBindingSet::~CurveBindingSet() = default;

    CurveBindingSet::CurveBindingSet(const CurveBindingSet &other) = default;

    CurveBindingSet &CurveBindingSet::operator =(const CurveBindingSet &other) = default;

    CurveBindingSet::CurveBindingSet(CurveBindingSet &&other) noexcept = default;

    CurveBindingSet &CurveBindingSet::operator =(CurveBindingSet &&other) noexcept = default;

    UnityClasses::GameObject *CurveBindingSet::findChildGameObject(UnityClasses::GameObject *parent, const std::string_view &path) {
        auto currentTransform = transformComponentOf(parent);

        std::string_view currentPath(path);
        while(!currentPath.empty()) {
            std::string_view nodeToFind;

            auto delim = currentPath.find('/');
            if(delim == std::string_view::npos) {
                nodeToFind = currentPath;
                currentPath = std::string_view();
            } else {
                nodeToFind = currentPath.substr(0, delim);
                currentPath = currentPath.substr(delim + 1);
            }

            for(const auto &child: currentTransform->m_Children) {
                if(child) {
                    if(child->UnityTypes::Transform::m_GameObject->m_Name == nodeToFind) {
                        currentTransform = child;
                        goto found;
                    }
                }
            }

            {
                std::stringstream stream;
                stream << "Unable to resolve path '" << path << "': node '" << parent->m_Name << "' has no child named '" << nodeToFind << "'";
                throw std::runtime_error(stream.str());
            }

            found: ;
        }

        return currentTransform->UnityTypes::Transform::m_GameObject;
    }


    UnityClasses::Transform *CurveBindingSet::transformComponentOf(UnityClasses::GameObject *gameObject) {
        for(const auto &component: gameObject->m_Component) {
            if(component.component) {
                auto casted = object_cast<UnityClasses::Transform>(component.component);
                if(casted) {
                    return casted;
                }
            }
        }

        throw std::logic_error("game object has no transform component");
    }

}
