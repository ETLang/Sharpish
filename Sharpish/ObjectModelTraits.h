#pragma once

#include "TypeID.h"

namespace CS
{
	template<class T>
	class com_ptr;

	template<class T>
	class weak_ptr;
}


namespace CSDetails
{
	enum TypeCategory
	{
		TC_Primitive,
		TC_Value,
		TC_Reference
	};
};

// __Ref is a traits class describing how a data type could be referenced.
// The default implementation describes reference types.
template<class T>
struct __Ref_Base
{
	static const CSDetails::TypeCategory Category = CSDetails::TC_Reference;
	static const bool IsValueType = false;
	static const bool IsPrimitive = false;
	static const bool IsInterface = false;
	static const bool IsWeakReference = false;
	typedef T Raw;
	typedef T* Simple;
	typedef CS::com_ptr<T> Strong;
	typedef CS::weak_ptr<T> Weak;
	typedef T* Fwd;
};

template<class T>
struct __Ref : public __Ref_Base<T>
{
};

// Specializing __Ref for each of the forms for reference types makes them stable... 
// __Ref<T>::*** and __Ref<__Ref<T>::Raw>::*** should always be the same type!
// __Ref<Object>::Strong and __Ref<Object*>::Strong and __Ref<weak_ptr<Object>>::Strong are all the same!
template<class T>
struct __Ref<T*> : public __Ref<T> { };

template<class T>
struct __Ref<T* const> : public __Ref<T> { };

template<class T>
struct __Ref<const T&> : public __Ref<T> { };

template<class T>
struct __Ref<T&> : public __Ref<T> { };

template<class T>
struct __Ref<CS::com_ptr<T>> : public __Ref<T>
{
	typedef CS::com_ptr<T> Simple;
};

template<class T>
struct __Ref<CS::weak_ptr<T>> : public __Ref<T> { typedef CS::com_ptr<T> Simple; static const bool IsWeakReference = true; };

template<class T>
struct __Ref<CS::com_ptr<T>*> : public __Ref<T> { typedef CS::com_ptr<T> Simple; };

template<class T>
struct __Ref<CS::com_ptr<T>&> : public __Ref<T> { typedef CS::com_ptr<T> Simple; };

// Handle void
template<>
struct __Ref<void>
{
	typedef void Raw;
	typedef void Simple;
	typedef void Strong;
	typedef void Weak;
	typedef void Fwd;
};

template<>
struct __Ref<void*>
{
	typedef void* Raw;
	typedef void* Simple;
	typedef void* Strong;
	typedef void* Weak;
	typedef void* Fwd;
};

template<>
struct __Ref<void**>
{
	typedef void** Raw;
	typedef void** Simple;
	typedef void** Strong;
	typedef void** Weak;
	typedef void** Fwd;
};

// _U is a traits class defining the usage of data types in various contexts.
// In a managed language like C# this is handled automatically, with rules for
// reference types and value types. 
// In C++ there's a lot of ways to go about it, but in order for the object model
// to be standardized across a variety of platforms, we need to define a consistent
// usage model similar to those managed languages.
// 
// The object model cares about:
// - The type as a returned value from a method
// - The type as an input parameter to a method
// - The type as an output parameter to a method
// - The type as an in/out parameter to a method
// - The type stored in a data structure (on the heap, basically)
//
// The template and its specializations below try to codify behavior similar to the rules for
// reference and value types in C#, while maintaining low overhead wherever possible.

// The default implementation of _U applies to all types except weak references.
template<class T>
struct _U
{
	// Methods returning T should a simple reference (pointer) to it
	typedef typename __Ref<T>::Simple Ret;

	// Methods taking T as input should take a simple reference
	typedef typename __Ref<T>::Simple In;

	// Methods taking T as an output parameter should take a pointer to a simple reference
	typedef typename __Ref<T>::Simple* Out;

	// In/Out parameters are treated the same as pure output parameters.
	typedef typename __Ref<T>::Simple* InOut;

	// Specifies how the data element should be stored on the heap.
	typedef typename __Ref<T>::Strong Storage;

	typedef typename __Ref<T>::Fwd Fwd;

	// Gets the default input parameter value (for ref types, this will be null. for value types, it'll be a const ref to a plainly contructed instance)
	static In DefaultArgValue() { return Storage(); }// usage_cast_in<T>(Storage());
};

// _U_Output specializes _U for method output parameters. The input type becomes identical to the In/Out type to reflect
// the fact that both usages were specified.
template<class T> struct _U_Output : public _U<T>
{
	typedef typename _U<T>::InOut In;
};

// This specialization handles weak references. Note that they work the same as strong ref types, except for how they are stored. 
// Hopefully that winds up being a pleasant convenience, rather than something that gets in the way.
// If a weak ref is stored somewhere, and the referenced object is destroyed, getting the stored value just returns null.
template<class T>
struct _U<CS::weak_ptr<T>> : public _U<T>
{
	typedef typename __Ref<T>::Strong Ret;
	typedef typename __Ref<T>::Strong* Out;
	typedef typename __Ref<T>::Strong* InOut;
	typedef typename __Ref<T>::Weak Storage;
	typedef typename __Ref<T>::Fwd Fwd;

	// Gets the default input parameter value (for ref types, this will be null. for value types, it'll be a const ref to a plainly contructed instance)
	//static In DefaultArgValue() { return usage_cast_in<T>(Storage()); }
};

// Specialization for value type input parameters
template<class T> struct _U<const T&> : public _U<T> { };

// Specialization for reference type input parameters.
// The signature matches primitive/value-type output parameters, so those get further specialized by the macros defined below.
template<class T> struct _U<T*> : public _U<T> { };

// Specialization for reference type output parameters
template<class T> struct _U<T**> : public _U_Output<T> { };

// Specialization for reference type output parameters
template<class T> struct _U<CS::com_ptr<T>*> : public _U_Output<CS::com_ptr<T>> { };

// Handle void and void*
template<>
struct _U<void>
{
	typedef void Ret;
	typedef void In;
	typedef void Out;
	typedef void InOut;
	typedef void Storage;
	typedef void Fwd;
};

template<>
struct _U<void*>
{
	typedef void* Ret;
	typedef void* In;
	typedef void* Out;
	typedef void* InOut;
	typedef void* Storage;
	typedef void* Fwd;

	static void* DefaultArgValue() { return nullptr; }
};

template<>
struct _U<void**>
{
	typedef void** Ret;
	typedef void** In;
	typedef void** Out;
	typedef void** InOut;
	typedef void** Storage;
	typedef void** Fwd;

	static void** DefaultArgValue() { return nullptr; }
};

// This namespace defines templates for a bunch of specializations on __Ref for primitives, value types, interfaces, generics, etc.
// They get used by the IS_PRIMITIVE, IS_VALUETYPE, IS_GENERICREF, etc. macros.
namespace CSDetails
{
	// Interfaces are like any other reference type, but knowning that they are
	// interfaces allows certain features to optimize differently (see box_cast)
	template<typename T>
	struct InterfaceRef : public __Ref_Base<T>
	{
		static const bool IsInterface = true;
	};

	// primitive types aren't referenced anywhere, really. Just copied.
	template<typename T>
	struct PrimitiveRef : public __Ref_Base<T>
	{
		static const bool IsValueType = true;
		static const bool IsPrimitive = true;
		typedef T Simple;
		typedef T Strong;
		typedef T Weak;
		typedef T&& Fwd;
	};

	// Value types are similar to primitives. They get copied most places. Their simple reference
	// type is a const reference, in an attempt to match C#'s concept of value types.
	template<typename T>
	struct ValueRef : public __Ref_Base<T>
	{
		static const bool IsValueType = true;
		typedef const T& Simple;
		typedef T Strong;
		typedef T Weak;
		typedef T&& Fwd;
	};

	template<typename T>
	struct Value_U
	{
		typedef typename __Ref<T>::Raw Ret;
		typedef typename __Ref<T>::Simple In;
		typedef typename __Ref<T>::Raw* Out;
		typedef typename __Ref<T>::Raw* InOut;
		typedef typename __Ref<T>::Strong Storage;
		typedef typename __Ref<T>::Fwd Fwd;

		// Gets the default input parameter value (for ref types, this will be null. for value types, it'll be a const ref to a plainly contructed instance)
		static In DefaultArgValue() { static T DefaultInstance; return DefaultInstance; }
	};
}

////////////////////////////////
// These macros assist in defining specializations of __Ref for primitive types, value types, and interfaces.

#define IS_INTERFACE(Typename) \
		template<> struct __Ref<Typename> : public ::CSDetails::InterfaceRef<Typename> { }; 

#define IS_PRIMITIVE(Typename, IDSTR) \
		DECLARETYPEID(Typename, IDSTR) \
		template<> struct __Ref<Typename> : public ::CSDetails::PrimitiveRef<Typename> { }; \
		template<> struct _U<Typename*> : public _U_Output<Typename> { };

#define IS_VALUETYPE(Typename, IDSTR) \
		DECLARETYPEID(Typename, IDSTR) \
		template<> struct __Ref<Typename> : public ::CSDetails::ValueRef<Typename> { }; \
		template<> struct _U<Typename> : public ::CSDetails::Value_U<Typename> { }; \
		template<> struct _U<Typename*> : public _U_Output<Typename> { };

#define __ARG_FMT(x,N) x##N
#define __DECL_FMT(x,N) class x##N

#define __ARG(x,N) __CHAIN(x,N,__ARG)
#define __DECL(x,N) __CHAIN(x,N,__DECL)

#define IS_GENERIC_REFTYPE(ArgCount,TypeName,ID) \
		DECLAREGENERICID(ArgCount, TypeName, ID) 

#define IS_GENERIC_INTERFACE(ArgCount,TypeName,ID) \
		DECLAREGENERICID(ArgCount, TypeName, ID) \
		template<__DECL(G,ArgCount)> struct __Ref<TypeName<__ARG(G,ArgCount)>> : \
			public ::CSDetails::InterfaceRef<TypeName<__ARG(G,ArgCount)>> { };

#define IS_GENERIC_VALUETYPE(ArgCount,TypeName,ID) \
		DECLAREGENERICID(ArgCount, TypeName, ID) \
		template<__DECL(G,ArgCount)> struct __Ref<TypeName<__ARG(G,ArgCount)>> : \
			public ::CSDetails::ValueRef<TypeName<__ARG(G,ArgCount)>> { }; \
		/*template<__DECL(G,ArgCount)> struct __Ref<const TypeName<__ARG(G,ArgCount)>&> : public __Ref<TypeName<__ARG(G,ArgCount)>> { };*/ \
		template<__DECL(G,ArgCount)> struct _U<TypeName<__ARG(G,ArgCount)>> : public ::CSDetails::Value_U<TypeName<__ARG(G,ArgCount)>> { }; \
		template<__DECL(G,ArgCount)> struct _U<TypeName<__ARG(G,ArgCount)>*> : public _U_Output<TypeName<__ARG(G,ArgCount)>> { };

#define IS_GENERIC_PRIMITIVE(ArgCount,TypeName,ID) \
		DECLAREGENERICID(ArgCount, TypeName, ID) \
		template<__DECL(G,ArgCount)> struct __Ref<TypeName<__ARG(G,ArgCount)>> : \
			public ::CSDetails::PrimitiveRef<TypeName<__ARG(G,ArgCount)> { }; \
		template<__DECL(G,ArgCount)> struct _U<TypeName<__ARG(G,ArgCount)>> : public _U_Output<T> { };