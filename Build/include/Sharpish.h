#pragma once

/**
 * \mainpage
 * 
 * \section Welcome
 * 
 * Sharpish is a C++ library that provides a variety of features that mimic constructs that are
 * ordinarily only found in C#. This allows someone familiar with C# to more easily work with C++
 * without having to rethink well-established solutions to common problems.
 *
 * Sharpish offers the following features:
 * * **Safe Runtime Type Casting** - A cross-platform, template-based implementation of IUnknown with
 * functions to easily 'cast' from one pointer type to another. Unlike dynamic_cast, this works across
 * DLL boundaries. This was originally based on KO Software's ["Implementing COM Interfaces with C++0x
 * Variadic Templates"] (https://www.codeproject.com/Articles/249257/Implementing-COM-Interfaces-with-Cplusplus0x-Varia)
 * on CodeProject.
 * * **Weak Referencing** - Permits weak referencing of object classes analogous to C#'s WeakReference.
 * * **Delegates** - A simple, fast callable delegate construct. Can be easily bound to class members,
 * static functions, std::function objects, and lambda expressions. This is a heavily modified and
 * enhanced version of Don Clugston's ["Fastest Possible C++ Delegate"]
 * (https://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible) on CodeProject.
 * * **Events** - Builds upon the Delegate construct to implement C++ events that are highly analogous 
 * to C# events.
 * * **Array<T>** - An array class that works much like any C# array. It can either manage its own
 * allocation, or use preallocated space, and carries length information with it.
 * * **Boxing** - Cast literally any type--primitives, structs, etc.--to a reference-counted object,
 * and safely cast it back again later. Just like casting anything to/from an object in C#.
 * * **Exceptions** - Extends std::exception to implement .Net equivalents: ArgumentException,
 * NullReferenceException, etc.
 * * **SIMD-Optimized Math** - Provides highly accelerated math objects you'd expect to find if you
 * were using Unity, XAML, or some other C#-based UX library: Float2, Float3, Float4, Quaternion,
 * Float4x3, Float4x4, etc.
 *
 * \section DnA Dependencies and Attribution
 * 
 * Original FastDelegate Copyright (c) 2004 Don Clugston
 * [http://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible]
 * 
 * Original COM Interface Template Implementation Copyright (c) 2011, KO Software (official@ko-sw.com)
 * [https://www.codeproject.com/Articles/249257/Implementing-COM-Interfaces-with-Cplusplus0x-Varia]
 *
 * \section Lic MIT Licensing
 * 
 * Copyright (c) 2020 Evan Lang
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

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
#if !defined(_MSVC_LANG) || _MSVC_LANG < 201703L
    typedef unsigned char byte;
#else
    using byte = std::byte;
#endif
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
