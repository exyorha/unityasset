#include <UnityAsset/Environment/Downcastable.h>

#include <UnityAsset/SerializedAsset/SerializedType.h>
#include <UnityAsset/UnityTypes.h>

namespace UnityAsset {

    Downcastable::Downcastable() = default;
    Downcastable::~Downcastable() = default;

    std::unique_ptr<Downcastable> Downcastable::loadObject(const UnityAsset::SerializedType &type, const Stream &data) {
        if(type.m_ScriptTypeIndex >= 0 || type.m_ScriptID.has_value()) {
            fprintf(stderr, "Downcastable::loadObject: object of type %d cannot be loaded because it has script data attached\n",
                    type.classID);

            return nullptr;
        }

        auto it = m_loaders.find(type.classID);
        if(it == m_loaders.end()) {
            fprintf(stderr, "Downcastable::loadObject: cannot deserialize an object of type %d\n",
                    type.classID);
            return nullptr;
        }

        return it->second(data);
    }

    template<typename T>
    std::unique_ptr<Downcastable> Downcastable::deserialize(const Stream &stream) {
        auto ptr = std::make_unique<T>();
        ptr->deserialize(stream);

        return ptr;
    }

    std::unordered_map<int32_t, Downcastable::Loader> Downcastable::m_loaders{
        { UnityClasses::GameObject::ClassID,               deserialize<UnityClasses::GameObject>     },
        { UnityClasses::Transform::ClassID,                deserialize<UnityClasses::Transform>      },
        { UnityClasses::Material::ClassID,                 deserialize<UnityClasses::Material>       },
        { UnityClasses::MeshRenderer::ClassID,             deserialize<UnityClasses::MeshRenderer>   },
        { UnityClasses::Texture2D::ClassID,                deserialize<UnityClasses::Texture2D>      },
        { UnityClasses::OcclusionCullingSettings::ClassID, deserialize<UnityClasses::OcclusionCullingSettings> },
        { UnityClasses::MeshFilter::ClassID,               deserialize<UnityClasses::MeshFilter>     },
        { UnityClasses::Mesh::ClassID,                     deserialize<UnityClasses::Mesh>           },
        { UnityClasses::Shader::ClassID,                   deserialize<UnityClasses::Shader>       },
        { UnityClasses::MeshCollider::ClassID,             deserialize<UnityClasses::MeshCollider>   },
        { UnityClasses::BoxCollider::ClassID,              deserialize<UnityClasses::BoxCollider>    },
        { UnityClasses::Cubemap::ClassID,                  deserialize<UnityClasses::Cubemap>        },
        { UnityClasses::Avatar::ClassID,                   deserialize<UnityClasses::Avatar>        },
        { UnityClasses::Animator::ClassID,                 deserialize<UnityClasses::Animator>        },
        { UnityClasses::RenderSettings::ClassID,           deserialize<UnityClasses::RenderSettings> },
        { UnityClasses::Light::ClassID,                    deserialize<UnityClasses::Light>          },
        { UnityClasses::MonoScript::ClassID,               deserialize<UnityClasses::MonoScript>     },
        { UnityClasses::SkinnedMeshRenderer::ClassID,      deserialize<UnityClasses::SkinnedMeshRenderer>    },
        { UnityClasses::AssetBundle::ClassID,              deserialize<UnityClasses::AssetBundle>    },
        { UnityClasses::PreloadData::ClassID,              deserialize<UnityClasses::PreloadData>    },
        { UnityClasses::LightmapSettings::ClassID,         deserialize<UnityClasses::LightmapSettings> },
        { UnityClasses::NavMeshSettings::ClassID,          deserialize<UnityClasses::NavMeshSettings> },
        { UnityClasses::ReflectionProbe::ClassID,          deserialize<UnityClasses::ReflectionProbe> },
        { UnityClasses::LightProbeGroup::ClassID,          deserialize<UnityClasses::LightProbeGroup> },
        { UnityClasses::NavMeshData::ClassID,              deserialize<UnityClasses::NavMeshData> },
        { UnityClasses::LightProbes::ClassID,              deserialize<UnityClasses::LightProbes> },
        { UnityClasses::OcclusionCullingData::ClassID,     deserialize<UnityClasses::OcclusionCullingData> },
    };

}
