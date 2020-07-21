# Sharpish
Sharpish is a C++ library that provides a variety of features that mimic constructs that are ordinarily only found in C#. This allows someone familiar with C# to more easily work with C++ without having to rethink well-established solutions to common problems.

Sharpish offers the following features:
* **Safe Runtime Type Casting** - A cross-platform, template-based implementation of IUnknown with functions to easily 'cast' from one pointer type to another. Unlike dynamic_cast, this works across DLL boundaries.
* **Weak Referencing** - Permits weak referencing of object classes analogous to C#'s WeakReference.
* **Delegates** - A simple, fast callable delegate construct. Can be easily bound to class members, static functions, std::function objects, and lambda expressions. This is a heavily modified and enhanced version of Don Clugston's ["Fastest Possible C++ Delegate"](https://www.codeproject.com/Articles/7150/Member-Function-Pointers-and-the-Fastest-Possible) on CodeProject.
* **Events** - Builds upon the Delegate construct to implement C++ events that are highly analogous to C# events.
* **Array<T>** - An array class that works much like any C# array. It can either manage its own allocation, or use preallocated space, and carries length information with it.
* **Boxing** - Cast literally any type--primitives, structs, etc.--to a reference-counted object, and safely cast it back again later. Just like casting anything to/from an object in C#.
* **Exceptions** - Extends std::exception to implement .Net equivalents: ArgumentException, NullReferenceException, etc.
* **SIMD-Optimized Math** - Provides highly accelerated math objects you'd expect to find if you were using Unity, XAML, or some other C#-based UX library: Float2, Float3, Float4, Quaternion, Float4x3, Float4x4, etc.

# Compatibility

Right now, Sharpish only supports MSVC targeting x86 and x64 platforms. Help supporting other compilers and targets is welcome and appreciated.

# Setup

The Build folder contains the latest build of Sharpish. To incorporate it into your project, reference the appropriate folders, add **Sharpish.lib** and include **Sharpish.h**.
Sharpish uses the CS:: namespace.

# Samples
TODO

# Guide
TODO

# License
 Sharpish is licensed under the MIT license.
 Copyright (c) 2020 Evan Lang
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
