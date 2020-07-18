/*****************************************************************//**
 * \file   Array.h
 * \brief  Header defining CS::Array
 * 
 * \author Evan Lang
 * \date   July 2020
 *********************************************************************/
#pragma once

#include <memory>

namespace CS
{
    /// An array whose size is initialized upon creation.
    /** 
     * `Array<T>` is analogous to a C# array. The length is established when the array is created,
     * and remains fixed thereafter. It is intelligent about reference counted types, so `Array<IUnknown>`
     * is equivalent to `Array<com_ptr<IUnknown>>`. It is capable of either allocating and managing its own
     * underlying memory, or simply wrapping preallocated memory and leaving memory management to the caller.
     * 
     * Like C#, Array objects are implicitly pass-by-reference. This makes `Array<T>` simple and performant
     * when passing as an argument or assigning to a field. Consider the following:
     * 
     *     Array<int> a = { 1, 2 };
     *     Array<int> b = a;
     *     b[0] = 3;
     * 
     * At this point, `a[0] == 3` because `b` references the same underlying memory as `a`.
     * 
     * `Array<T>` is intelligent about whether it manages the allocation of the underlying memory or not.
     * If it is constructed to wrap preallocated memory, it will not attempt to deallocate that memory later.
     * Otherwise, it will manage the allocation as a reference-counted block.
     * 
     * \param T The type of array
     */
    template<typename T>
    class Array final
    {
    public:
        typedef T_RAW(T) ElementType;
        typedef T_STORE(T) ElementContainerType;
        typedef T_STORE(T) value_type;

        /// Constructs a null array
        Array() : _ptr(nullptr), _length(0) { }

        /// Constructs a null array
        Array(nullptr_t) : _ptr(nullptr), _length(0) { }

        /**
         * \brief Constructs an array of the specified length.
         * 
         * \param size The number of elements of the desired array.
         */
        Array(size_t size) : _ptr(new T_STORE(T)[size]), _mgr(_ptr, std::default_delete<T_STORE(T)[]>()), _length(size) { }

        /**
         * \brief Constructs an array that wraps preallocated memory.
         * 
         * The memory will not be deallocated by the array.
         * 
         * \param wrapped The memory to wrap. Note that for reference-counted types this must be `com_ptr<T>*`
         * \param size The number of elements in the array.
         */
        Array(T_STORE(T)* wrapped, size_t size) : _ptr(wrapped), _length(size) { }

        /**
         * \brief Constructs an array from an initializer list..
         * 
         * \param lst The list of values to initialize the array with.
         */
        Array(std::initializer_list<T> lst) : _ptr(new T_STORE(T)[lst.size()]), _length(lst.size())
        {
            int i = 0;
            for (auto& x : lst)
            {
                _ptr[i] = x;
                i++;
            }
        }

        /**
         * \brief Constructs an array from an iterator range.
         * 
         * The values of the iterated collection are copied into the array.
         * The iterator distance must be measurable (must overload `operator-`, like vector iterators, not map iterators).
         * 
         * \param start Iterator to the first element of the array.
         * \param end Iterator after the last element of the array.
         */
        template<typename Iter>
        Array(const Iter& start, const Iter& end) : _length(end - start), _ptr(new T_STORE(T)[_length]), _mgr(_ptr, std::default_delete<T_STORE(T)[]>())
        {
            int i = 0;
            for (auto iter = start; iter != end; iter++)
            {
                _ptr[i] = *iter;
                i++;
            }
        }

        Array<T>& operator =(const Array<T>& rhs) { _ptr = rhs._ptr; _mgr = rhs._mgr; _length = rhs._length; return *this; }
        Array<T>& operator =(nullptr_t) { _ptr = nullptr; _mgr.reset(); _length = 0; return *this; }

        /// Indexer to an array element.
        T_STORE(T)& operator [](int index) const { return _ptr[index]; }
        T_STORE(T)& operator [](short index) const { return _ptr[index]; }
        T_STORE(T)& operator [](int64_t index) const { return _ptr[index]; }
        T_STORE(T)& operator [](unsigned int index) const { return _ptr[index]; }
        T_STORE(T)& operator [](unsigned short index) const { return _ptr[index]; }
        T_STORE(T)& operator [](uint64_t index) const { return _ptr[index]; }

        typedef T_STORE(T)* iterator;
        typedef const T_STORE(T)* const_iterator;

        iterator begin() { return _ptr; }
        const_iterator begin() const { return _ptr; }
        iterator end() { return _ptr + _length; }
        const_iterator end() const { return _ptr + _length; }
        size_t size() const { return _length; }
        bool empty() const { return _length == 0; }

        bool operator ==(const Array<T>& other) const { return _ptr == other._ptr; }
        bool operator !=(const Array<T>& other) const { return _ptr != other._ptr; }

        operator bool() const { return !!_ptr; }
        bool operator !() const { return !_ptr; }

        /** Converts the array to an equivalent `std::vector<T>`. */
        std::vector<T> vec() { return std::vector<T>(begin(), end()); }

    private:
        size_t _length;
        T_STORE(T)* _ptr;
        std::shared_ptr<T_STORE(T)> _mgr;

    };
}

IS_GENERIC_VALUETYPE(1, ::CS::Array, "B5495742-AEDB-469E-A8C6-1BBDBF45E6D9");
