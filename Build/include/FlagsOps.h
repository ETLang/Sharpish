#pragma once
 
#include <type_traits>

template <class T>
using is_enum_class = std::integral_constant<bool, !std::is_convertible<T, int>::value
	&& std::is_enum<T>::value>;

template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint64_t), T>::type operator |(T lhs, T rhs) { return (T)((uint64_t)lhs | (uint64_t)rhs); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint32_t), T>::type operator |(T lhs, T rhs) { return (T)((uint32_t)lhs | (uint32_t)rhs); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint16_t), T>::type operator |(T lhs, T rhs) { return (T)((uint16_t)lhs | (uint16_t)rhs); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint8_t), T>::type operator |(T lhs, T rhs) { return (T)((uint8_t)lhs | (uint8_t)rhs); }

template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint64_t), T>::type operator &(T lhs, T rhs) { return (T)((uint64_t)lhs & (uint64_t)rhs); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint32_t), T>::type operator &(T lhs, T rhs) { return (T)((uint32_t)lhs & (uint32_t)rhs); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint16_t), T>::type operator &(T lhs, T rhs) { return (T)((uint16_t)lhs & (uint16_t)rhs); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint8_t), T>::type operator &(T lhs, T rhs) { return (T)((uint8_t)lhs & (uint8_t)rhs); }

template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint64_t), T>::type operator ^(T lhs, T rhs) { return (T)((uint64_t)lhs ^ (uint64_t)rhs); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint32_t), T>::type operator ^(T lhs, T rhs) { return (T)((uint32_t)lhs ^ (uint32_t)rhs); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint16_t), T>::type operator ^(T lhs, T rhs) { return (T)((uint16_t)lhs ^ (uint16_t)rhs); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint8_t), T>::type operator ^(T lhs, T rhs) { return (T)((uint8_t)lhs ^ (uint8_t)rhs); }

template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint64_t), T>::type operator ~(T value) { return (T)(~(uint64_t)value); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint32_t), T>::type operator ~(T value) { return (T)(~(uint32_t)value); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint16_t), T>::type operator ~(T value) { return (T)(~(uint16_t)value); }
template<class T>
typename std::enable_if<is_enum_class<T>::value && sizeof(T) == sizeof(uint8_t), T>::type operator ~(T value) { return (T)(~(uint8_t)value); }
