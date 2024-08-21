#ifndef UNITY_ASSET_ENVIRONMENT_OBJECT_POINTER_H
#define UNITY_ASSET_ENVIRONMENT_OBJECT_POINTER_H

namespace UnityAsset {

    class Downcastable;

    template<typename T>
    class ObjectPointer {
    public:
        inline operator bool() const {
            return m_ptr;
        }

        inline T &operator *() const {
            return *m_ptr;
        }

        inline T *operator ->() const {
            return m_ptr;
        }

        inline void link(T *ptr) {
            m_ptr = ptr;
        }

        inline operator T *() const {
            return m_ptr;
        }

        inline T *get() const {
            return m_ptr;
        }

        template<typename Other>
        inline bool operator ==(const ObjectPointer<Other> &pointer) const {
            return static_cast<const Downcastable *>(get()) == static_cast<const Downcastable *>(pointer.get());
        }

    private:
        T *m_ptr = nullptr;
    };

}

#endif

