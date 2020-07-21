#pragma once

// com_ptr<> is a template class modified from Microsoft's ComPtr<> implementation.
// It permits casting without modifying an object's reference count.
// The appropriate usage pattern is to use a com_ptr<> object as a member of your data model,
// and pointers within the bodies of functions.

#include "BasicInterfaces.h"
#include "WeakReference.h"
#include <assert.h>

#pragma warning(push)
// 'class' : default constructor could not be generated
#pragma warning(disable: 4510)
// object 'class' can never be instantiated - user-defined constructor required
#pragma warning(disable: 4610)
//'derived class' : destructor could not be generated because a base class destructor is inaccessible
#pragma warning(disable: 4624)
// Warnings 4510 and 4610 are generated because of as<>() helper object
// that don't require any constructors thus disabling those warnings
// Warning 4624 is generated if the object has private destructor and the object cannot be created on the stack
// Comptr does not require the object to be created on the stack thus disabling the warning

namespace CS {

    namespace Details
    {

        struct BoolStruct
        {
            int Member;
        };

        typedef int BoolStruct::* BoolType;

        // Helper object that makes IUnknown methods private
        template <typename T>
        class RemoveIUnknownBase : public T
        {
        private:
            // STDMETHOD macro implies virtual.
            // com_ptr can be used with any class that implements the 3 methods of IUnknown.
            // com_ptr does not require these methods to be virtual.
            // When com_ptr is used with a class without a virtual table, marking the functions
            // as virtual in this class adds unnecessary overhead.
            HRESULT __stdcall QueryInterface(REFIID riid, _COM_Outptr_ void **ppvObject);
            ULONG __stdcall AddRef();
            ULONG __stdcall Release();
            void* __stdcall CastTo(REFIID riid);
        };

        template<typename T>
        struct RemoveIUnknown
        {
            typedef RemoveIUnknownBase<T> ReturnType;
        };

        template<typename T>
        struct RemoveIUnknown<const T>
        {
            typedef const RemoveIUnknownBase<T> ReturnType;
        };

        template <typename T> // T should be the com_ptr<T> or a derived type of it, not just the interface
        class ComPtrRefBase
        {
        public:
            typedef typename T::InterfaceType InterfaceType;

            operator IObject**() const throw()
            {
                static_assert(__is_base_of(IObject, InterfaceType), "Invalid cast: InterfaceType does not derive from IObject");
                return reinterpret_cast<IObject**>(_ptr->ReleaseAndGetAddressOf());
            }

        protected:
            T* _ptr;
        };

        template <typename T>
        class StoredRef : public Details::ComPtrRefBase<T> // T should be the com_ptr<T> or a derived type of it, not just the interface
        {
        public:
            StoredRef(_In_opt_ T* ptr) throw()
            {
                ComPtrRefBase<T>::_ptr = ptr;
            }

            // Conversion operators
            operator void**() const throw()
            {
                return reinterpret_cast<void**>(_ptr->ReleaseAndGetAddressOf());
            }

            // This is our operator com_ptr<U> (or the latest derived class from com_ptr (e.g. weak_ref))
            operator T*() throw()
            {
                *_ptr = nullptr;
                return _ptr;
            }

            // We define operator InterfaceType**() here instead of on ComPtrRefBase<T>, since
            // if InterfaceType is IUnknown or IInspectable, having it on the base will colide.
            operator InterfaceType**() throw()
            {
                return _ptr->ReleaseAndGetAddressOf();
            }

            // This is used for IID_PPV_ARGS in order to do __uuidof(**(ppType)).
            // It does not need to clear  _ptr at this point, it is done at IID_PPV_ARGS_Helper(StoredRef&) later in this file.
            InterfaceType* operator *() throw()
            {
                return _ptr->Get();
            }

            // Explicit functions
            InterfaceType* const * GetAddressOf() const throw()
            {
                return _ptr->GetAddressOf();
            }

            InterfaceType** ReleaseAndGetAddressOf() throw()
            {
                return _ptr->ReleaseAndGetAddressOf();
            }
        };

    }

    class weak_ref;

    /**
     * \brief Smart pointer reference to a reference-counted object
     * \param T Type of object being referenced. Must implement IUnknown.
     * 
     * A smart pointer works like a pointer, with the added feature of 
     * automatically calling AddRef and Release on the object it references.
     * 
     * This implementation is largely borrowed from Microsoft's [ComPtr]
     * (https://docs.microsoft.com/en-us/cpp/cppcx/wrl/comptr-class?view=vs-2019)
     * class to be made portable.
     */
    template <typename T>
    class com_ptr
    {
    public:
        typedef T InterfaceType;

    protected:
        InterfaceType *_ptr;
        template<class U> friend class com_ptr;

        void InternalAddRef() const throw()
        {
            if (_ptr != nullptr)
                reinterpret_cast<IUnknown*>(_ptr)->AddRef();
        }

        unsigned long InternalRelease() throw()
        {
            unsigned long ref = 0;
            auto temp = reinterpret_cast<IUnknown*>(_ptr);

            if (temp != nullptr)
            {
                _ptr = nullptr;
                ref = temp->Release();
            }

            return ref;
        }

    public:
#pragma region constructors
        /** Constructs a null pointer. */
        com_ptr() throw() : _ptr(nullptr) { }

        /** Constructs a null pointer. */
        com_ptr(nullptr_t) throw() : _ptr(nullptr) { }

        /**
         * Constructs from a raw pointer.
         * 
         * \param other A raw pointer (must be castable to T)
         */
        template<class U>
        com_ptr(_In_opt_ U *other) throw() : _ptr(other)
        {
            InternalAddRef();
        }

        /** Copy constructor. */
        com_ptr(const com_ptr& other) throw() : _ptr(other._ptr)
        {
            InternalAddRef();
        }

        /** Copy constructor where U is castable to T. */
        template<class U>
        com_ptr(const com_ptr<U> &other, typename std::enable_if<std::is_convertible<U*, T*>::value, void *>::type * = 0) throw() :
            _ptr(other._ptr)
        {
            InternalAddRef();
        }

        /** Move constructor. */
        com_ptr(_Inout_ com_ptr &&other) throw() : _ptr(nullptr)
        {
            if (this != reinterpret_cast<com_ptr*>(&reinterpret_cast<byte&>(other)))
            {
                Swap(other);
            }
        }

        /** Move constructor where U is castable to T. */
        template<class U>
        com_ptr(_Inout_ com_ptr<U>&& other, typename std::enable_if<std::is_convertible<U*, T*>::value, void *>::type * = 0) throw() :
            _ptr(other._ptr)
        {
            other._ptr = nullptr;
        }
#pragma endregion

        ~com_ptr() throw()
        {
            InternalRelease();
        }

#pragma region assignment
        /** Assign to null. */
        com_ptr& operator=(nullptr_t) throw()
        {
            InternalRelease();
            return *this;
        }

        /** Assign from raw pointer. */
        com_ptr& operator=(_In_opt_ T *other) throw()
        {
            if (_ptr != other)
            {
                com_ptr(other).Swap(*this);
            }
            return *this;
        }

        /**
         * \brief Assign from raw pointer, where U is castable to T.
         * 
         * \param U The other pointer type (must be castable to T)
         */
        template <typename U>
        com_ptr& operator=(_In_opt_ U *other) throw()
        {
            com_ptr(other).Swap(*this);
            return *this;
        }

        /** Assign from com_ptr. */
        com_ptr& operator=(const com_ptr &other) throw()
        {
            if (_ptr != other._ptr)
            {
                com_ptr(other).Swap(*this);
            }
            return *this;
        }

        /**
         * \brief Assign from com_ptr, where U is castable to T.
         * 
         * \param U The other pointer type (must be castable to T)
         */
        template<class U>
        com_ptr& operator=(const com_ptr<U>& other) throw()
        {
            com_ptr(other).Swap(*this);
            return *this;
        }

        /** R-Value Assign from com_ptr. */
        com_ptr& operator=(_Inout_ com_ptr &&other) throw()
        {
            com_ptr(static_cast<com_ptr&&>(other)).Swap(*this);
            return *this;
        }

        /**
         * \brief R-Value Assign from com_ptr, where U is castable to T.
         * 
         * \param U The other pointer type (must be castable to T)
         */
        template<class U>
        com_ptr& operator=(_Inout_ com_ptr<U>&& other) throw()
        {
            com_ptr(static_cast<com_ptr<U>&&>(other)).Swap(*this);
            return *this;
        }

        /** Dereference pointer */
        T& operator *() throw()
        {
            return *Get();
        }

#pragma endregion

#pragma region modifiers
        
        /**
         * \brief R-Value swap two pointers.
         * 
         * Swapping can save performance by avoiding redundant AddRef/Release calls.
         * 
         * \param r Other pointer being swapped
         */
        void Swap(_Inout_ com_ptr&& r) throw()
        {
            T* tmp = _ptr;
            _ptr = r._ptr;
            r._ptr = tmp;
        }

        /**
         * \brief Swap two pointers.
         * 
         * Swapping can save performance by avoiding redundant AddRef/Release calls.
         *
         * \param r Other pointer being swapped
         */
        void Swap(_Inout_ com_ptr& r) throw()
        {
            T* tmp = _ptr;
            _ptr = r._ptr;
            r._ptr = tmp;
        }
#pragma endregion

        operator Details::BoolType() const throw()
        {
            return Get() != nullptr ? &Details::BoolStruct::Member : nullptr;
        }

        bool operator==(const T* rhs) const
        {
            return _ptr == rhs;
        }

        /** Get the raw pointer */
        T* Get() const throw()
        {
            return _ptr;
        }

        typename Details::RemoveIUnknown<InterfaceType>::ReturnType* operator->() const throw()
        {
            return static_cast<typename Details::RemoveIUnknown<InterfaceType>::ReturnType*>(_ptr);
        }

        Details::StoredRef<com_ptr<T>> operator&() throw()
        {
            return Details::StoredRef<com_ptr<T>>(this);
        }

        const Details::StoredRef<const com_ptr<T>> operator&() const throw()
        {
            return Details::StoredRef<const com_ptr<T>>(this);
        }

        /** Get the address of the raw pointer (pass to QueryInterface) */
        T* const* GetAddressOf() const throw()
        {
            return &_ptr;
        }

        /** Get the address of the raw pointer (pass to QueryInterface) */
        T** GetAddressOf() throw()
        {
            return &_ptr;
        }

        /** Release existing value and get the address of the raw pointer (pass to QueryInterface) */
        T** ReleaseAndGetAddressOf() throw()
        {
            InternalRelease();
            return &_ptr;
        }

        /**
         * \brief Disassociates this ComPtr object from the interface that it represents.
         * 
         * \return The disassociated pointer
         */
        T* Detach() throw()
        {
            T* ptr = _ptr;
            _ptr = nullptr;
            return ptr;
        }

        /**
         * \brief Associates this ComPtr with the interface type specified by the current template type parameter.
         * 
         * \param other The pointer to associate
         */
        void Attach(_In_opt_ InterfaceType* other) throw()
        {
            if (_ptr != nullptr)
            {
                auto ref = reinterpret_cast<IUnknown*>(_ptr)->Release();
                (ref);
                // Attaching to the same object only works if duplicate references are being coalesced. Otherwise
                // re-attaching will cause the pointer to be released and may cause a crash on a subsequent dereference.
                assert(ref != 0 || _ptr != other);
            }

            _ptr = other;
        }

        /** Equivalent to setting the pointer to null. */
        unsigned long Reset()
        {
            return InternalRelease();
        }

        // query for U interface
        template<typename U>
        U* CastTo() const throw()
        {
            return (U*)_ptr->CastTo(_uuidof(U));
        }

        template<>
        T* CastTo<T>() const throw()
        {
            return _ptr;
        }

        /**
         * \brief Returns a ComPtr object that represents the interface identified by the specified template parameter.
         * 
         * \param p [Out] The returned pointer
         * \param U The interface type to query for
         * \return S_OK if successful; otherwise an error code.
         */
        template<typename U>
        HRESULT As(_Inout_ Details::StoredRef<com_ptr<U>> p) const throw()
        {
            return reinterpret_cast<IUnknown*>(_ptr)->QueryInterface(_uuidof(U), p);
        }

        /**
         * \brief Returns a ComPtr object that represents the interface identified by the specified template parameter.
         *
         * \param p [Out] The returned pointer
         * \param U The interface type to query for
         * \return S_OK if successful; otherwise an error code.
         */
        template<typename U>
        HRESULT As(_Out_ com_ptr<U>* p) const throw()
        {
            return reinterpret_cast<IUnknown*>(_ptr)->QueryInterface(_uuidof(U), reinterpret_cast<void**>(p->ReleaseAndGetAddressOf()));
        }

        // query for riid interface and return as IObject
        /**
         * \brief Returns a ComPtr object that represents the interface identified by the unique identifier.
         *
         * \param riid The unique ID of the interface to query for
         * \param p [Out] The returned pointer
         * \return S_OK if successful; otherwise an error code.
         */
        HRESULT AsIID(REFIID riid, _Out_ com_ptr<IObject>* p) const throw()
        {
            return reinterpret_cast<IUnknown*>(_ptr)->QueryInterface(riid, reinterpret_cast<void**>(p->ReleaseAndGetAddressOf()));
        }

        HRESULT AsWeak(_Out_ weak_ref* pWeakRef) const throw()
        {
            return AsWeak(_ptr, pWeakRef);
        }
    };    // com_ptr

    class weak_ref : public com_ptr<Ext::IWeakReference>
    {
    private:
        void operator->();
    protected:
        HRESULT InternalResolve(REFIID riid, _Outptr_result_maybenull_ void** obj) throw()
        {
            *obj = nullptr;

            if (_ptr == nullptr)
            {
                // Weak reference was already released
                return S_OK;
            }

            HRESULT hr = _ptr->Resolve(riid, obj);

            if (SUCCEEDED(hr) && *obj == nullptr)
            {
                // Release weak reference because it will not succeed ever
                InternalRelease();
            }

            return hr;
        }

        HRESULT InternalResolve(REFIID riid, _Outptr_result_maybenull_ void** obj) const throw()
        {
            *obj = nullptr;

            if (_ptr == nullptr)
            {
                // Weak reference was already released
                return S_OK;
            }

            return _ptr->Resolve(riid, obj);
        }

    public:
        Details::StoredRef<weak_ref> operator&() throw()
        {
            return Details::StoredRef<weak_ref>(this);
        }

        const Details::StoredRef<const weak_ref> operator&() const throw()
        {
            return Details::StoredRef<const weak_ref>(this);
        }

        weak_ref() throw() : com_ptr(nullptr)
        {
        }

        weak_ref(nullptr_t) throw() : com_ptr(nullptr)
        {
        }

        weak_ref(_In_opt_ Ext::IWeakReference* ptr) throw() : com_ptr(ptr)
        {
        }

        weak_ref(const com_ptr<Ext::IWeakReference>& ptr) throw() : com_ptr(ptr)
        {
        }

        weak_ref(const weak_ref& ptr) throw() : com_ptr(ptr)
        {
        }

        weak_ref(_Inout_ weak_ref&& ptr) throw() : com_ptr(static_cast<com_ptr<Ext::IWeakReference>&&>(ptr))
        {
        }

        ~weak_ref() throw()
        {
        }

        // Resolve U interface
        template<typename U>
        HRESULT As(Details::StoredRef<com_ptr<U>> ptr) throw()
        {
            static_assert(!std::is_same<Ext::IWeakReference, U>::value, "Details:: cannot resolve Details:: object.");
            static_assert(std::is_base_of<IObject, U>::value, "weak_ref::As() can only be used on types derived from IObject");

            return InternalResolve(_uuidof(U), ptr);
        }

        template<typename U>
        HRESULT As(Details::StoredRef<com_ptr<U>> ptr) const throw()
        {
            static_assert(!Details::IsSame<Ext::IWeakReference, U>::value, "Details:: cannot resolve Details:: object.");
            static_assert(__is_base_of(IObject, U), "weak_ref::As() can only be used on types derived from IObject");

            return InternalResolve(_uuidof(U), ptr);
        }

        template<typename U>
        HRESULT As(_Out_ com_ptr<U>* ptr) throw()
        {
            static_assert(!Details::IsSame<Details::Ext::IWeakReference, U>::value, "Details:: cannot resolve Details:: object.");
            static_assert(__is_base_of(IInspectable, U), "weak_ref::As() can only be used on types derived from IInspectable");

            return InternalResolve(_uuidof(U), ptr->ReleaseAndGetAddressOf());
        }

        template<typename U>
        HRESULT As(_Out_ com_ptr<U>* ptr) const throw()
        {
            static_assert(!Details::IsSame<Details::Ext::IWeakReference, U>::value, "Details:: cannot resolve Details:: object.");
            static_assert(__is_base_of(IInspectable, U), "weak_ref::As() can only be used on types derived from IInspectable");

            return InternalResolve(_uuidof(U), ptr->ReleaseAndGetAddressOf());
        }

        HRESULT AsIID(REFIID riid, _Out_ com_ptr<IUnknown>* ptr) throw()
        {
            assert(riid != __uuidof(Ext::IWeakReference));

            return InternalResolve(riid, reinterpret_cast<void**>(ptr->ReleaseAndGetAddressOf()));
        }

        HRESULT CopyTo(REFIID riid, _Outptr_result_maybenull_ void** ptr) throw()
        {
            assert(riid != __uuidof(Ext::IWeakReference));

            return InternalResolve(riid, ptr);
        }

        template<typename U>
        HRESULT CopyTo(_Outptr_result_maybenull_ U** ptr) throw()
        {
            static_assert(__is_base_of(IInspectable, U), "weak_ref::CopyTo() can only be used on types derived from IInspectable");

            return InternalResolve(_uuidof(U), reinterpret_cast<IInspectable**>(ptr));
        }

        HRESULT CopyTo(_Outptr_result_maybenull_ Ext::IWeakReference** ptr) throw()
        {
            InternalAddRef();
            *ptr = _ptr;
            return S_OK;
        }
    };

    // Microsoft's weak_ref is lacking a bit because it doesn't carry any implicit type of its own,
    // and a const weak_ref is basically useless. We'll fix that!
    //
    // Microsoft's is never const because if the pointer fails to resolve it autoreleases
    // (changing the ref count and internal pointer). The const version of Resolve() here
    // doesn't do that, so it works, but until Resolve() is called in a non-const context,
    // it'll retain the weak reference and keep querying it.
    //
    // This implementation also does not depend on IInspectable


    /**
     * \brief A weak reference to a reference-counted object.
     * 
     * Unlike a smart pointer, the weak reference cannot be treated as an analog of a pointer.
     * Call `Resolve()` on a weak reference to get a `com_ptr<T>` reference to the object.
     * 
     * If the object has been destroyed, `Resolve()' will return null. Always check that the
     * result of `Resolve()` is not null before using it.
     * 
     * \param T The object type being referenced.
     */
    template<class T>
    class weak_ptr : public weak_ref
    {
    public:
        /** Constructs a null pointer. */
        weak_ptr() throw() : weak_ref(nullptr) { }

        /** Constructs a null pointer. */
        weak_ptr(nullptr_t) throw() : weak_ref(nullptr) { }

        /** Constructs from an IWeakReferenceSource */
        weak_ptr(_In_opt_ Ext::IWeakReferenceSource* ptr) throw() : weak_ref(nullptr)
        {
            if (ptr != nullptr)
                ptr->GetWeakReference(GetAddressOf());
        }

        /**
         * \brief Constructs from an object.
         * 
         * \param ptr Pointer to a live object to get a weak reference of. Must not be null.
         */
        template<class T>
        weak_ptr(_In_opt_ T* ptr) throw() : weak_ref(nullptr)
        {
            if (ptr != nullptr)
                static_cast<Ext::IWeakReferenceSource*>(ptr)->GetWeakReference(&_ptr);
        }

        /**
         * \brief Constructs from an object.
         * 
         * \param U The object pointer type (must be castable to T)
         * \param ptr Pointer to a live object to get a weak reference of. Must not be null.
         */
        template<class U, std::enable_if_t<std::is_base_of_v<Ext::IWeakReferenceSource, U>, int> = 0>
        weak_ptr(const com_ptr<U>& ptr) throw() : weak_ref(nullptr)
        {
            if (ptr != nullptr)
                static_cast<Ext::IWeakReferenceSource*>(ptr.Get())->GetWeakReference(&_ptr);
        }

        template<class U, std::enable_if_t<!std::is_base_of_v<Ext::IWeakReferenceSource, U>, int> = 0>
        weak_ptr(const com_ptr<U>& ptr) throw() : weak_ref(nullptr)
        {
            if (ptr != nullptr)
            {
                com_ptr<Ext::IWeakReferenceSource> wref;
                ptr.As(&wref);

                if (wref)
                    wref->GetWeakReference(&_ptr);
            }
        }

        /** Copy constructor */
        weak_ptr(const weak_ptr& ptr) throw() : weak_ref(ptr) { }

        /** Move constructor */
        weak_ptr(_Inout_ weak_ptr&& ptr) throw() : weak_ref(ptr) { }

        ~weak_ptr() throw() { }

        /**
         * \brief Resolve a weak reference to a different type.
         * 
         * \param U The interface type to resolve from the object
         * \return A com_ptr to the resolved object, or null if the object has been destroyed or cannot be cast to U.
         */
        template<class U>
        com_ptr<U> Resolve() const
        {
            com_ptr<U> out;

            // I argue that the const_cast is perfectly reasonable here, because const is 
            // an organizational tool for developers; the compiler doesn't really use it to optimize anything.
            // Hence, it's more important to preserve the *spirit* of constness than the techical definition.
            // A reference count and internal weak reference stuff are perfect examples of something that can change
            // without violating that spirit.
            if (_ptr != nullptr)
                if (S_OK == _ptr->Resolve(_uuidof(U), reinterpret_cast<void**>(out.GetAddressOf())) && !out)
                    const_cast<weak_ptr<T>*>(this)->InternalRelease();

            return out;
        }

        /**
         * \brief Resolve a weak reference.
         * 
         * \return A com_ptr to the resolved object, or null if the object has been destroyed.
         */
        com_ptr<T> Resolve() const
        {
            return Resolve<T>();
        }

        weak_ptr<T>* operator &() throw()
        {
            return this;
        }

#pragma region assignment
        /** Assign to null */
        weak_ptr& operator=(nullptr_t) throw()
        {
            InternalRelease();
            return *this;
        }

        /** Assign from IWeakReference */
        weak_ptr& operator=(_In_opt_ Ext::IWeakReference *other) throw()
        {
            if (_ptr != other)
            {
                weak_ptr(other).Swap(*this);
            }
            return *this;
        }

        //template <typename U>
        //weak_ptr& operator=(_In_opt_ U *other) throw()
        //{
        //    weak_ptr(other).Swap(*this);
        //    return *this;
        //}

        /** Assign from another weak_ptr */
        weak_ptr& operator=(const weak_ptr &other) throw()
        {
            if (_ptr != other._ptr)
            {
                weak_ptr(other).Swap(*this);
            }
            return *this;
        }

        /**
         * \brief Assign from a weak_ptr of a different type
         * 
         * \param U Pointer type being assigned from. Must be castable to T.
         */
        template<class U>
        weak_ptr& operator=(const weak_ptr<U>& other) throw()
        {
            weak_ptr(other).Swap(*this);
            return *this;
        }

        /** Assign from another weak_ptr */
        weak_ptr& operator=(_Inout_ weak_ptr &&other) throw()
        {
            weak_ptr(static_cast<weak_ptr&&>(other)).Swap(*this);
            return *this;
        }

        /**
         * \brief Assign from a weak_ptr of a different type
         *
         * \param U Pointer type being assigned from. Must be castable to T.
         */
        template<class U>
        weak_ptr& operator=(_Inout_ weak_ptr<U>&& other) throw()
        {
            weak_ptr(static_cast<weak_ptr<U>&&>(other)).Swap(*this);
            return *this;
        }

        T& operator *() throw()
        {
            return *Get();
        }

#pragma endregion
    };

    template<typename T>
    HRESULT AsWeak(_In_ T* p, _Out_ weak_ref* pWeak) throw()
    {
        static_assert(!Details::IsSame<Details::, T>::value, "Cannot get Details:: object to Details::.");
        com_ptr<Details::Source> refSource;

        HRESULT hr = p->QueryInterface(IID_PPV_ARGS(refSource.GetAddressOf()));
        if (FAILED(hr))
        {
            return hr;
        }

        com_ptr<Details::> weakref;
        hr = refSource->GetWeakReference(weakref.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        *pWeak = weak_ref(weakref);
        return S_OK;
    }

    // Comparation operators - don't compare COM object identity
    template<class T, class U>
    bool operator==(const com_ptr<T>& a, const com_ptr<U>& b) throw()
    {
        static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
        return a.Get() == b.Get();
    }

    template<class T>
    bool operator==(const com_ptr<T>& a, nullptr_t) throw()
    {
        return a.Get() == nullptr;
    }

    template<class T>
    bool operator==(nullptr_t, const com_ptr<T>& a) throw()
    {
        return a.Get() == nullptr;
    }

    template<class T, class U>
    bool operator!=(const com_ptr<T>& a, const com_ptr<U>& b) throw()
    {
        static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
        return a.Get() != b.Get();
    }

    template<class T>
    bool operator!=(const com_ptr<T>& a, nullptr_t) throw()
    {
        return a.Get() != nullptr;
    }

    template<class T>
    bool operator!=(nullptr_t, const com_ptr<T>& a) throw()
    {
        return a.Get() != nullptr;
    }

    template<class T, class U>
    bool operator<(const com_ptr<T>& a, const com_ptr<U>& b) throw()
    {
        static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
        return a.Get() < b.Get();
    }

    //// Details::StoredRef comparisons
    template<class T, class U>
    bool operator==(const Details::StoredRef<com_ptr<T>>& a, const Details::StoredRef<com_ptr<U>>& b) throw()
    {
        static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
        return a.GetAddressOf() == b.GetAddressOf();
    }

    template<class T>
    bool operator==(const Details::StoredRef<com_ptr<T>>& a, nullptr_t) throw()
    {
        return a.GetAddressOf() == nullptr;
    }

    template<class T>
    bool operator==(nullptr_t, const Details::StoredRef<com_ptr<T>>& a) throw()
    {
        return a.GetAddressOf() == nullptr;
    }

    template<class T>
    bool operator==(const Details::StoredRef<com_ptr<T>>& a, void* b) throw()
    {
        return a.GetAddressOf() == b;
    }

    template<class T>
    bool operator==(void* b, const Details::StoredRef<com_ptr<T>>& a) throw()
    {
        return a.GetAddressOf() == b;
    }

    template<class T, class U>
    bool operator!=(const Details::StoredRef<com_ptr<T>>& a, const Details::StoredRef<com_ptr<U>>& b) throw()
    {
        static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
        return a.GetAddressOf() != b.GetAddressOf();
    }

    template<class T>
    bool operator!=(const Details::StoredRef<com_ptr<T>>& a, nullptr_t) throw()
    {
        return a.GetAddressOf() != nullptr;
    }

    template<class T>
    bool operator!=(nullptr_t, const Details::StoredRef<com_ptr<T>>& a) throw()
    {
        return a.GetAddressOf() != nullptr;
    }

    template<class T>
    bool operator!=(const Details::StoredRef<com_ptr<T>>& a, void* b) throw()
    {
        return a.GetAddressOf() != b;
    }

    template<class T>
    bool operator!=(void* b, const Details::StoredRef<com_ptr<T>>& a) throw()
    {
        return a.GetAddressOf() != b;
    }

    template<class T, class U>
    bool operator<(const Details::StoredRef<com_ptr<T>>& a, const Details::StoredRef<com_ptr<U>>& b) throw()
    {
        static_assert(__is_base_of(T, U) || __is_base_of(U, T), "'T' and 'U' pointers must be comparable");
        return a.GetAddressOf() < b.GetAddressOf();
    }

}


namespace std
{
    template<typename T>
    struct hash<CS::com_ptr<T>>
    {
        size_t operator()(CS::com_ptr<T> const& d) const
        {
            return (size_t)d.Get();
        }
    };

    template<typename T>
    struct hash<CS::weak_ptr<T>>
    {
        size_t operator()(CS::weak_ptr<T> const& d) const
        {
            return (size_t)d.();
        }
    };
}

#pragma warning(pop)