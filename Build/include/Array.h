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
        /** Element container type. For ComObjects, this com_ptr<T>. For other types, it's just T. */
        typedef T_STORE(T) ElementContainerType;
        typedef T_RAW(T) ElementType;
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
        Array(size_t size) : _ptr(new ElementContainerType[size]), _mgr(_ptr, std::default_delete<ElementContainerType[]>()), _length(size) { }

        /**
         * \brief Constructs an array that wraps preallocated memory.
         * 
         * The memory will not be deallocated by the array.
         * 
         * \param wrapped The memory to wrap. Note that for reference-counted types this must be `com_ptr<T>*`
         * \param size The number of elements in the array.
         */
        Array(ElementContainerType* wrapped, size_t size) : _ptr(wrapped), _length(size) { }

        /**
         * \brief Constructs an array from an initializer list..
         * 
         * \param lst The list of values to initialize the array with.
         */
        Array(std::initializer_list<T> lst) : _ptr(new ElementContainerType[lst.size()]), _length(lst.size())
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
        Array(const Iter& start, const Iter& end) : _length(end - start), _ptr(new ElementContainerType[_length]), _mgr(_ptr, std::default_delete<ElementContainerType[]>())
        {
            int i = 0;
            for (auto iter = start; iter != end; iter++)
            {
                _ptr[i] = *iter;
                i++;
            }
        }

        /**  Assigns an array reference. */
        Array<T>& operator =(const Array<T>& rhs) { _ptr = rhs._ptr; _mgr = rhs._mgr; _length = rhs._length; return *this; }
        /**  Assigns an array reference to null. */
        Array<T>& operator =(nullptr_t) { _ptr = nullptr; _mgr.reset(); _length = 0; return *this; }

        /** Indexer to an array element.. */
        ElementContainerType& operator [](int index) const { return _ptr[index]; }
        ElementContainerType& operator [](short index) const { return _ptr[index]; }
        ElementContainerType& operator [](int64_t index) const { return _ptr[index]; }
        ElementContainerType& operator [](unsigned int index) const { return _ptr[index]; }
        ElementContainerType& operator [](unsigned short index) const { return _ptr[index]; }
        ElementContainerType& operator [](uint64_t index) const { return _ptr[index]; }

        typedef T_STORE(T)* iterator;
        typedef const T_STORE(T)* const_iterator;

        /**  Gets an iterator at the beginning of the array. */
        iterator begin() { return _ptr; }
        /**  Gets a const iterator at the beginning of the array. */
        const_iterator begin() const { return _ptr; }
        /**  Gets an iterator after the end of the array. */
        iterator end() { return _ptr + _length; }
        /**  Gets a const iterator after the end of the array. */
        const_iterator end() const { return _ptr + _length; }
        /**  Gets the size of the array. */
        size_t size() const { return _length; }
        /**  Gets whether the array has no elements. Also returns true if the array is null. */
        bool empty() const { return _length == 0; }

        /**  Gets whether two `Array<T>`s reference the same array. */
        bool operator ==(const Array<T>& other) const { return _ptr == other._ptr; }
        /**  Gets whether two `Array<T>`s don't reference the same array. */
        bool operator !=(const Array<T>& other) const { return _ptr != other._ptr; }

        /**  Returns true if the array is non-null. */
        operator bool() const { return !!_ptr; }
        /**  Returns true if the array is null. */
        bool operator !() const { return !_ptr; }

        /** Converts the array to an equivalent `std::vector<T>`. */
        std::vector<T> vec() { return std::vector<T>(begin(), end()); }

    private:
        size_t _length;
        ElementContainerType* _ptr;
        std::shared_ptr<ElementContainerType> _mgr;

    };
}

IS_GENERIC_VALUETYPE(1, ::CS::Array, "B5495742-AEDB-469E-A8C6-1BBDBF45E6D9");
