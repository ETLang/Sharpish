#pragma once

// Make.h is modified from Microsoft's implementation of Make for WRL.

#include "WeakReference.h"
#include <unordered_map>

namespace CS
{
	namespace Details {

		// Memory allocation for objects supporting weak references
	// It allocates memory for WeakReference and T object. 
	// Besides it make partial initialization of T object - set the _weakRef variable 
		template<typename T>
		class MakeAllocator
		{
		public:
			MakeAllocator() throw() : _buffer(nullptr)
			{
			}

			~MakeAllocator() throw()
			{
				if (_buffer != nullptr)
				{
					delete _buffer;
				}
			}

			void* Allocate() throw()
			{
				assert(_buffer == nullptr);

				// Allocate memory with operator new(size, nothrow) only
				// This will allow developer to override one operator only
				// to enable different memory allocation model
				_buffer = operator new (sizeof(T), std::nothrow);
				if (_buffer == nullptr)
				{
					return nullptr;
				}

				// Create the weak reference object with new that will use
				// simple MakeAllocator without weak reference support
				void* weakbuffer = operator new (sizeof(WeakReference), std::nothrow);
				if (weakbuffer == nullptr)
				{
					return nullptr;
				}

				// Cast the raw pointer to activatable object T*
				T* obj = reinterpret_cast<T*>(_buffer);
				// Assign the weak reference to the object
				// in case of exception beeing thrown in T the destructor of 
				// RuntimeClass will release the object
				// The weak reference is initialized prior to constructor call
				// because WRL cannot handle any exceptions in constructor
				obj->_weakRef = new (weakbuffer)WeakReference;

				return _buffer;
			}

			void Detach() throw()
			{
				_buffer = nullptr;
			}
		private:
			void* _buffer;
		};
	}


	// Make and MakeAndInitialize funtions must not be makred as throw()
	// This enables compiler to generate exception handling code that will
	// execute the allocator object destructor which will prevent 
	// from memory leaks in case of exception
	template <typename T>
	com_ptr<T> Make()
	{
		static_assert(std::is_base_of<IUnknown, T>::value, "Make can only instantiate types that derive from CS::IUnknown");

		com_ptr<T> object;
		// Using MakeAllocator helper to prevent memory leaks if object throws during construction
		Details::MakeAllocator<T> allocator;

		void* buffer = allocator.Allocate();
		if (buffer == nullptr)
			return object;

		auto ptr = new (buffer)T; // Calls placement new form of T. Should be public even for DontUseNewUseMake
		object.Attach(ptr);
		// Object instantiation succeeded thus we detach memory from allocator
		// in case of failure allocator object would destroy memory in its destructor
		allocator.Detach();

		//ReportObjectAlloc(object.Get());
		return object;
	}

#define MakeHelper(...) \
		static_assert(std::is_base_of<IUnknown, T>::value, "Make can only instantiate types that derive from IUnknown"); \
		com_ptr<T> object; \
		Details::MakeAllocator<T> allocator; \
		void *buffer = allocator.Allocate(); \
		if (buffer == nullptr) \
		{ \
			return object; \
		} \
		auto ptr = new (buffer)T(__VA_ARGS__); \
		object.Attach(ptr); \
		allocator.Detach(); \
		return object;

	template <typename T, typename TArg1>
	com_ptr<T> Make(TArg1&& arg1)
	{
		MakeHelper(std::forward<TArg1>(arg1))
	}

	template <typename T, typename TArg1, typename TArg2>
	com_ptr<T> Make(TArg1&& arg1, TArg2&& arg2)
	{
		MakeHelper(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2))
	}

	template <typename T, typename TArg1, typename TArg2, typename TArg3>
	com_ptr<T> Make(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3)
	{
		MakeHelper(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3))
	}

	template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
	com_ptr<T> Make(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4)
	{
		MakeHelper(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3), std::forward<TArg4>(arg4))
	}

	template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
	com_ptr<T> Make(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5)
	{
		MakeHelper(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3), std::forward<TArg4>(arg4), std::forward<TArg5>(arg5))
	}

	template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
	com_ptr<T> Make(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5, TArg6&& arg6)
	{
		MakeHelper(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3), std::forward<TArg4>(arg4),
			std::forward<TArg5>(arg5), std::forward<TArg6>(arg6))
	}

	template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
	com_ptr<T> Make(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5, TArg6&& arg6, TArg7&& arg7)
	{
		MakeHelper(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3), std::forward<TArg4>(arg4),
			std::forward<TArg5>(arg5), std::forward<TArg6>(arg6), std::forward<TArg7>(arg7))
	}

	template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
	com_ptr<T> Make(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5, TArg6&& arg6, TArg7&& arg7, TArg8&& arg8)
	{
		MakeHelper(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3), std::forward<TArg4>(arg4),
			std::forward<TArg5>(arg5), std::forward<TArg6>(arg6), std::forward<TArg7>(arg7), std::forward<TArg8>(arg8))
	}

	template <typename T, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8, typename TArg9>
	com_ptr<T> Make(TArg1&& arg1, TArg2&& arg2, TArg3&& arg3, TArg4&& arg4, TArg5&& arg5, TArg6&& arg6, TArg7&& arg7, TArg8&& arg8, TArg9&& arg9)
	{
		MakeHelper(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2), std::forward<TArg3>(arg3), std::forward<TArg4>(arg4),
			std::forward<TArg5>(arg5), std::forward<TArg6>(arg6), std::forward<TArg7>(arg7), std::forward<TArg8>(arg8), std::forward<TArg9>(arg9))
	}
}