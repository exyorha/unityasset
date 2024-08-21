#ifndef UNITY_ASSET_ENVIRONMENT_OBJECT_FACTORY_H
#define UNITY_ASSET_ENVIRONMENT_OBJECT_FACTORY_H

#include <memory>

namespace UnityAsset {

    class Downcastable;
    class SerializedType;
    class Stream;

    std::unique_ptr<Downcastable> loadObject(const SerializedType &type, const Stream &data);


}

#endif
