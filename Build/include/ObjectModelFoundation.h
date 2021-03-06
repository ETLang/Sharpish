#pragma once

#include "BasicInterfaces.h"
#include "ComBox.h"
#include "TypeIDAssoc.h"
//#include "EveEnums.h"
#include "FastDelegate.h"
#include <type_traits>

template<typename T, typename U>
typename T* runtime_cast(U* ptr);

template<typename T, typename U>
typename const T* runtime_cast(const U* ptr);

template<typename T, typename U>
typename T* runtime_cast(const CS::com_ptr<U>& ptr);

namespace CS
{
#pragma region box_cast - casts a variable of any known type to a BoxedValue. Primitives and value types get wrapped in a ComBox.

	template<class T>
	typename std::enable_if<std::is_same<T, void>::value, BoxedValue>::type box_cast(void)
	{
		// void is a no-op
		return nullptr;
	}

	// value types get boxed when cast to unknown
	template<class T>
	typename std::enable_if<__Ref<T>::IsValueType, BoxedValue>::type box_cast(const T& x)
	{
		return Make<ComBox<T>>(x);
	}

	// reference types don't need much done to them to cast to objects.
	// If it's an interface, query IUnknown to get the base pointer of the object.
	// If it's not an interface, the input pointer *is* the base pointer, so increment the ref count and return.
	template<class T>
	typename std::enable_if<!__Ref<T>::IsValueType&& __Ref<T>::IsInterface, BoxedValue>::type box_cast(T* x)
	{
		return runtime_cast<IObject>(x);
	}

	template<class T>
	typename std::enable_if<!__Ref<T>::IsValueType && !__Ref<T>::IsInterface, BoxedValue>::type box_cast(T* x)
	{
		return reinterpret_cast<IObject*>(x);
	}

	template<class T>
	typename std::enable_if<!__Ref<T>::IsValueType&& __Ref<T>::IsInterface, BoxedValue>::type box_cast(const com_ptr<T>& x)
	{
		return runtime_cast<IObject>(x);
	}

	template<class T>
	typename std::enable_if<!__Ref<T>::IsValueType && !__Ref<T>::IsInterface, BoxedValue>::type box_cast(const com_ptr<T>& x)
	{
		return reinterpret_cast<IObject*>(x.Get());
	}

	template<class T>
	typename std::enable_if<!__Ref<T>::IsValueType, BoxedValue>::type box_cast(const weak_ptr<T>& x)
	{
		return x.Resolve<IObject>();
	}

#pragma endregion

#pragma region unbox_cast - casts a BoxedValue to its unboxed type. 

	template<class T>
	typename std::enable_if<std::is_same<T, void>::value, void>::type unbox_cast(const BoxedValue& obj)
	{
		// void is a no-op
	}

	template<class T>
	typename std::enable_if<__Ref<T>::IsValueType, typename __Ref<T>::Strong>::type unbox_cast(const BoxedValue& obj)
	{
		auto box = runtime_cast<ComBox<typename __Ref<T>::Raw>>(obj);

		if (!box)
			throw Exception("Failed to unbox value. Is the boxed value null, or a different type?");

		return box->Value;
	}

	template<class T>
	typename std::enable_if<!__Ref<T>::IsValueType && !__Ref<T>::IsWeakReference, typename __Ref<T>::Strong>::type unbox_cast(const BoxedValue& obj)
	{
		return runtime_cast<typename __Ref<T>::Raw>(obj);
	}

	template<class T>
	typename std::enable_if<__Ref<T>::IsWeakReference, weak_ptr<T>>::type unbox_cast(const BoxedValue& obj)
	{
		return weak_ptr<T>(runtime_cast<typename __Ref<T>::Raw>(obj));
	}

#pragma endregion

#pragma region usage_cast_in - Casts any variable to its appropriate form as an input parameter to a method.

	template<typename T>
	T_IN(T) usage_cast_in(typename __Ref<T>::Raw& x) { return (T_IN(T))x; }

	template<typename T>
	T_IN(T) usage_cast_in(typename __Ref<T>::Raw* x) { return (T_IN(T))x; }

	template<typename T>
	const T_IN(T) usage_cast_in(const typename __Ref<T>::Raw* x) { return (const T_IN(T))x; }

	template<typename T>
	T_IN(T) usage_cast_in(const com_ptr<typename __Ref<T>::Raw>& x) { return x.Get(); }

	template<typename T>
	T_IN(T) usage_cast_in(const weak_ptr<typename __Ref<T>::Raw>& x) {
		return x.Resolve().Get();
	}

	template<typename T>
	T_IN(T) usage_cast_in(const typename __Ref<T>::Raw& x) { return x; }

#pragma endregion

#pragma region usage_cast_return - Cast any type to its appropriate form as a return value from a method

	// Cast any type to its appropriate form as a return value from a method
	template<typename T>
	T_RETURN(T) usage_cast_return(typename __Ref<T>::Raw& x) { return (T_RETURN(T))x; }

	template<typename T>//, typename std::enable_if<std::is_base_of<IUnknown, typename __Ref<T>::Raw>::value, void>::type* = 0>
	typename std::enable_if<std::is_base_of<IUnknown, typename __Ref<T>::Raw>::value, T_RETURN(T)>::type usage_cast_return(typename __Ref<T>::Raw* x) { return (T_RETURN(T))x; }

	template<typename T>
	T_RETURN(T) usage_cast_return(const typename __Ref<T>::Raw& x) { return (T_RETURN(T))x; }

	template<typename T>
	T_RETURN(T) usage_cast_return(com_ptr<typename __Ref<T>::Raw>& x) { return x.Get(); }

	template<typename T>
	T_RETURN(T) usage_cast_return(const com_ptr<typename __Ref<T>::Raw>& x) { return x.Get(); }

	template<typename T>
	T_RETURN(T) usage_cast_return(weak_ptr<typename __Ref<T>::Raw>& x) { return x.Resolve().Get(); }

	template<typename T>
	T_RETURN(T) usage_cast_return(const weak_ptr<typename __Ref<T>::Raw>& x) { return x.Resolve().Get(); }

#pragma endregion
}