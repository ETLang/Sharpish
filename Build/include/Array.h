#pragma once

#include <memory>

namespace CS
{
    template<typename T>
    class Array sealed
    {
    public:
        typedef T_RAW(T) ElementType;
        typedef T_STORE(T) ElementContainerType;
        typedef T_STORE(T) value_type;

        Array() : _ptr(nullptr), _length(0) { }
        Array(nullptr_t) : _ptr(nullptr), _length(0) { }
        Array(size_t size) : _ptr(new T_STORE(T)[size]), _mgr(_ptr, std::default_delete<T_STORE(T)[]>()), _length(size) { }
        Array(T_STORE(T)* wrapped, size_t size) : _ptr(wrapped), _length(size) { }
        Array(std::initializer_list<T> lst) : _ptr(new T_STORE(T)[lst.size()]), _length(lst.size())
        {
            int i = 0;
            for (auto& x : lst)
            {
                _ptr[i] = x;
                i++;
            }
        }

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

        std::vector<T> vec() { return std::vector<T>(begin(), end()); }

    private:
        size_t _length;
        T_STORE(T)* _ptr;
        std::shared_ptr<T_STORE(T)> _mgr;

    };
}

IS_GENERIC_VALUETYPE(1, ::CS::Array, "B5495742-AEDB-469E-A8C6-1BBDBF45E6D9");
