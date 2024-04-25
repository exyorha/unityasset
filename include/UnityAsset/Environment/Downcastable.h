#ifndef UNITY_ASSET_ENVIRONMENT_DOWNCASTABLE_H
#define UNITY_ASSET_ENVIRONMENT_DOWNCASTABLE_H

#include <memory>
#include <unordered_map>

namespace UnityAsset {

    class Stream;

    class SerializedType;
    class LoadedSerializedAsset;

    class Downcastable {
    protected:
        Downcastable();

    public:
        virtual ~Downcastable();

        Downcastable(const Downcastable &other) = delete;
        Downcastable &operator =(const Downcastable &other) = delete;

        virtual int32_t classId() const = 0;
        virtual bool canBeCastTo(int32_t classId) const = 0;
        virtual void link(LoadedSerializedAsset *asset) = 0;

        static std::unique_ptr<Downcastable> loadObject(const UnityAsset::SerializedType &type, const Stream &data);

        template<typename T>
        inline bool isType() const {
            return classId() == T::ClassID;
        }

        template<typename T>
        inline bool isDerivedFrom() const {
            return canBeCastTo(T::ClassID);
        }

    private:
        template<typename T>
        static std::unique_ptr<Downcastable> deserialize(const Stream &stream);

        using Loader = std::unique_ptr<Downcastable> (*)(const Stream &stream);
        static std::unordered_map<int32_t, Loader> m_loaders;
    };

    template<typename T>
    T *object_cast(Downcastable *object) {
        if(!object)
            return nullptr;

        if(object->canBeCastTo(T::ClassID)) {
            return static_cast<T *>(object);
        } else {
            return nullptr;
        }
    }

    template<typename T>
    const T *object_cast(const Downcastable *object) {
        if(!object)
            return nullptr;

        if(object->canBeCastTo(T::ClassID)) {
            return static_cast<const T *>(object);
        } else {
            return nullptr;
        }
    }

    template<typename T>
    T &object_cast(Downcastable &object) {
        if(object.canBeCastTo(T::ClassID)) {
            return static_cast<T &>(object);
        } else {
            throw std::bad_cast();
        }
    }

    template<typename T>
    const T &object_cast(const Downcastable &object) {
        if(object.canBeCastTo(T::ClassID)) {
            return static_cast<const T &>(object);
        } else {
            throw std::bad_cast();
        }
    }
}

#endif

