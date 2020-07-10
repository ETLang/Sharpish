#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <DirectXMath.h>
#include <dxgitype.h>
#include <rpc.h>
#include <string>
#include <vector>

namespace CS
{
    //using namespace std;
    using namespace DirectX;
    typedef unsigned char byte;
    using string = std::string;
}

#if !defined(MIN)
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#if !defined(MAX)
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

// passes a macro argument containing commas as a single argument. Useful for using template types when passing types to a macro, like Dictionary<string,string>. 
#define AsIs(...) __VA_ARGS__
#define ASSTRING(...) #__VA_ARGS__
#define ASWSTRING(...) L#__VA_ARGS__
#define TOSTRING(...) ASSTRING(__VA_ARGS__)
#define TOWSTRING(...) ASWSTRING(__VA_ARGS__)

#define PROPERTY(type, prop) __declspec ( property ( put = Set##prop, get = Get##prop ) ) type prop
#define PROPERTY_READONLY(type, prop) __declspec(property(get=Get##prop)) type prop
#define PROPERTY_INDEXABLE(type, prop) __declspec ( property ( put = Set##prop, get = Get##prop ) ) type prop[]
#define PROPERTY_INDEXABLE_READONLY(type, prop) __declspec(property(get=Get##prop)) type prop[]

template<class T, size_t Size>
__forceinline size_t ArrayLength(const T(&)[Size]) { return Size; }

template<class T> T* malloc_array(size_t arrLength) { return (T*)malloc(sizeof(T)* arrLength); }
template<class T> T* _aligned_malloc_array(size_t arrLength, size_t align) { return (T*)_aligned_malloc(sizeof(T)* arrLength, align); }
template<class T> T* _aligned_malloc_array(size_t arrLength) { return (T*)_aligned_malloc(sizeof(T)* arrLength, __alignof(T)); }
template<class T> void zero(T&& item) { memset((void*)&item, 0, sizeof(T)); }
template<class T, int TSize> void zero(T itemarray[TSize]) { memset(itemarray, 0, sizeof(T) * TSize); }
template<class T> void zero(T* itemarray, int elements) { memset(itemarray, 0, sizeof(T) * elements); }

template<class T> void repeat(T* itemarray, int elements, const T& value)
{
    for (int i = 0; i < elements; i++)
        itemarray[i] = value;
}
template<class T, int TSize> void repeat(T itemarray[TSize], const T& value, int startIndex = 0, int count = -1)
{
    if (count == -1) count = TSize - startIndex;
    assert(startIndex + count <= TSize);

    repeat(itemarray + startIndex, count, value);
}

#include "FlagsOps.h"
#include "MacroRepeater.h"
#include "Hashing.h"
#include "CSException.h"

#include "BasicInterfaces.h"
#include "ObjectModelTraits.h"
#include "ComClass.h"
#include "com_ptr.h"
#include "weak_ptr.h"
#include "Make.h"
#include "TypeID.h"
#include "Object.h"
#include "Blob.h"

// These macros assist in identifying the input parameter, output parameter, storage, and return forms of templated types.
// If the type is known, you're best off using the appropriate form directly. 
#define T_IN(T) typename _U<T>::In
#define T_OUT(T) typename _U<T>::Out
#define T_INOUT(T) typename _U<T>::InOut
#define T_STORE(T) typename _U<T>::Storage
#define T_RETURN(T) typename _U<T>::Ret
#define T_RAW(T) typename __Ref<T>::Raw

// Gets the default value of a type
#define DEFAULT_(T) (_U<T>::DefaultArgValue())

template<typename T>
T Def() { static T _value; return _value; }

#include "FunctionTemplateHelp.h"
#include "ComBox.h"
#include "FastDelegate.h"
#include "FastEvent.h"
#include "ObjectModelFoundation.h"
#include "TypeIDAssoc.h"

#include "CriticalSection.h"
#include "ThreadSignal.h"
#include "Array.h"
#include "MathTypes.h"
#include "half.h"
#include "Range.h"
#include "Rational.h"
#include "Rect.h"
#include "Size.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "RFrame.h"

// Helpers
#include "FileHelper.h"
#include "StringHelper.h"
#include "MathHelper.h"
#include "ToString.h"

////////////////////////
// Object management
////////////////////////

// runtime_cast casts an object implementing IObject at runtime.
// Unlike QueryInterface, IObject::CastTo doesn't increase the reference count.
template<typename T, typename U>
typename T* runtime_cast(U* ptr) { return ptr ? (T*)ptr->CastTo((CS::Details::Identifier<T>::UUIDOf())) : (T*)nullptr; }

template<typename T, typename U>
typename const T* runtime_cast(const U* ptr) { return ptr ? reinterpret_cast<const T*>(ptr->CastTo(_uuidof(T))) : (const T*)nullptr; }

template<typename T, typename U>
typename T* runtime_cast(const CS::com_ptr<U>& ptr) { return ptr ? ptr.CastTo<T>() : (T*)nullptr; }


#define DELEGATE(Method) MakeDelegate(this, &__Ref<decltype(this)>::Raw::Method)
#define STATIC_DELEGATE(Method) MakeDelegate(&Method)