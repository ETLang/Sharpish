#pragma once

#include <vector>
#include "Make.h"
#include "ComClass.h"
#include "WeakReference.h"

typedef unsigned __int32 FourCC;

namespace CS
{
	class ComTypeBase
	{
	};

	template<typename BaseType = IWeakReferenceSource2, typename... Implements>
	class ComType : public ComEntry<BaseType, Implements...>, public ComTypeBase
	{
		typedef ComDiscovery<BaseType, Implements...> DiscoverableBase;
	protected:
		typedef ComEntry<BaseType, Implements...> Base;
	public:
		typedef ComType<BaseType, Implements...> Discoverable;

		//private:
		//	void* operator new(size_t) throw()
		//	{
		//		__WRL_ASSERT__(false);
		//		return 0;
		//	}

	public:
		//void* operator new(size_t, _In_ void* placement) throw()
		//{
		//	return placement;
		//}

		typedef void Interface;

	protected:
		template<typename T> friend class Details::MakeAllocator;

		Details::WeakReference* _weakRef;

	public:
		ComType()
#ifdef ENABLE_MEMORY_DEBUGGING
			: DebugWatch(false)
#endif
		{
			_weakRef = new Details::WeakReference();// (Details::WeakReference*)operator new (sizeof(Details::WeakReference), std::nothrow);
			_weakRef->SetUnknown(reinterpret_cast<IUnknown*>(this));
		}

		virtual ~ComType() { _weakRef->SetUnknown(nullptr); _weakRef->Release(); _weakRef = nullptr; }

		template<typename tSomething, typename ... tParams>
		ComType(tSomething&& first, tParams && ... theParams)
			: Base(std::forward<tSomething>(first), std::forward<tParams>(theParams)...)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif

		{
			_weakRef->SetUnknown(reinterpret_cast<IUnknown*>(this));
		}


	public:

#ifdef ENABLE_MEMORY_DEBUGGING
		bool DebugWatch;
#endif
		virtual ULONG STDMETHODCALLTYPE AddRef()
		{
			ULONG refCount = _weakRef->IncrementStrongReference();

#ifdef ENABLE_MEMORY_DEBUGGING
			BREAK_CONDITION(DebugWatch);
#endif

			return refCount;
		}

		virtual ULONG STDMETHODCALLTYPE Release()
		{
			ULONG aNumRefs;

			aNumRefs = _weakRef->DecrementStrongReference();

#ifdef ENABLE_MEMORY_DEBUGGING
			BREAK_CONDITION(DebugWatch);
#endif

			if (aNumRefs <= 0)
				delete this;

			return aNumRefs;
		}

		virtual void* __stdcall CastTo(REFIID riid)
		{
			if (_uuidof(IUnknown) == riid || _uuidof(IObject) == riid)
				return reinterpret_cast<void*>(this);
			else
			{
				void* ret;
				HRESULT aRes = DiscoverableBase::InternalQueryInterface(riid, this, &ret);
				if (FAILED(aRes))
					return nullptr;
				return ret;
			}
		}

		virtual HRESULT __stdcall GetTypeId(IID* outType)
		{
			return E_FAIL;
		}

		template<class Q>
		Q* CastTo() { return reinterpret_cast<Q*>(CastTo(_uuidof(Q))); }

		DECLARE_DISAMBIGUATION_CAST_INSTANCE(DiscoverableBase)

		template<class T>
		static HRESULT InternalQueryInterface(REFIID theIID, T* obj, void** theOut)
		{
			return DiscoverableBase::InternalQueryInterface(theIID, obj, theOut);
		}

		//! \brief    Top-level implementation of IUnknown::QueryInterface.
		//! 
		//! \remarks
		//! This method implies that the implementation class provides
		//! interface discovery information via the \a InternalQueryInterface
		//! static member function.
		STDMETHODIMP QueryInterface(REFIID theIID, void ** theOut)
		{
			if (theOut == nullptr) return E_POINTER;

			if (theIID == _uuidof(IUnknown) || theIID == _uuidof(IObject))
				*theOut = reinterpret_cast<void*>(this);
			else
			{
				HRESULT aRes = DiscoverableBase::InternalQueryInterface(theIID, this, theOut);
				if (FAILED(aRes))
				{
					*theOut = nullptr;
					return aRes;
				}
			}

			AddRef();
			return S_OK;
		}
	};

	namespace Details
	{
		//std::vector<void(*)()>& RSI(void(*si)());

		//template< void(*A)(), void(*B)() >
		//struct SameFunc { static const bool value = false; };

		//template<void(*A)()>
		//struct SameFunc<A, A> { static const bool value = true; };

		template< typename T>
		class HasStaticInitializer
		{
			/* SFINAE StaticInit has correct signature :) */
			template<typename A>
			static std::true_type test(void (A::*)()) {
				static_assert(false, "wow cool");
				return std::true_type();
			}

			/* SFINAE StaticInit exists :) */
			template <typename A>
			static decltype(test(&A::StaticInit))
				test(decltype(&A::StaticInit), void *) {
				/* StaticInit exists. What about sig? */
				typedef decltype(test(&A::StaticInit)) return_type;
				return return_type();
			}

			/* SFINAE game over :( */
			template<typename A>
			static std::false_type test(...) {
				return std::false_type();
			}

			/* This will be either `std::true_type` or `std::false_type` */
			typedef decltype(test<T>(0, 0)) type;

		public:
			//static const bool value = SameFunc<&T::StaticInit, &T::Base::StaticInit>::value; // type::value;
			//static bool isInitialized;
		};

	}

	/**
	 * \brief Template to assist implementing reference counted classes.
	 * \param Class The class type being implemented.
	 * \param BaseClass The base class that the defining class will inherit from.
	 * \param Implements A set of 0 or more interfaces to implement
	 * 
	 * Use ComObject to implement your own reference-counted class types.
	 * 
	 * The advantages to using ComObject include:
	 * - Automatic implementation of IUnknown and safe runtime type casting using `runtime_cast<T>()`.
	 * - Safe lifecycle management, using com_ptr
	 * - Ability to use weak referencing, using weak_ptr
	 * - Ability to request the root ID of the implementing type using GetTypeID
	 * 
	 * Reference-counted class types must declare an associated UUID.
	 * 
	 * When instantiating such classes, the proper method is to use `Make<T>()` instead of calling `new()`:
	 * `com_ptr<Car> newCar = Make<Car>();`
	 * 
	 * The following example declares a class Car, which derives from Object, followed by a class Truck
	 * that inherits Car:
	 * 
	 *     class comid("b7a9ea93-6b5e-4fde-9a6c-ea9872a00e7e") Car : 
	 *         public CS::ComObject<Car, Object>
	 *     {
	 *     };
	 * 
	 *     class comid("abcdef01-6b5e-4fde-9a6c-ea9872a00e7e") Truck :
	 *         public CS::ComObject<Truck, Car>
	 *     {
	 *     };
	 *
	 * 
	 */

	template<typename Class, typename BaseClass = ComType<>, typename... Implements>
	class ComObject : public BaseClass, public Implements...
	{
		template<class T>
		friend class Details::HasStaticInitializer;

		typedef ComDiscovery<BaseClass, Implements...> DiscoverableBase;
	protected:
		typedef ComObject<Class, BaseClass, Implements...> Base;

	public:
		typedef Base Discoverable;
		typedef Class Interface;

#ifdef ENABLE_MEMORY_DEBUGGING
		bool DebugWatch;
#endif

		ComObject()
#ifdef ENABLE_MEMORY_DEBUGGING
			: DebugWatch(false)
#endif
		{ }

		template<typename tFirst, typename ... tParams>
		ComObject(tFirst&& firstParam, tParams && ... theParams)
			: BaseClass(std::forward<tFirst>(firstParam), std::forward<tParams>(theParams)...)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{ }

	public:

		virtual HRESULT STDMETHODCALLTYPE GetWeakReference(Ext::IWeakReference **weakReference)
		{
			*weakReference = _weakRef;
			(*weakReference)->AddRef();
			return S_OK;
		}

		ULONG STDMETHODCALLTYPE AddRef()
		{
			ULONG refCount = _weakRef->IncrementStrongReference();

#ifdef ENABLE_MEMORY_DEBUGGING
			BREAK_CONDITION(DebugWatch);
#endif

			return refCount;
		}

		ULONG STDMETHODCALLTYPE Release()
		{
			ULONG aNumRefs;

			aNumRefs = _weakRef->DecrementStrongReference();

#ifdef ENABLE_MEMORY_DEBUGGING
			BREAK_CONDITION(DebugWatch);
#endif

			if (aNumRefs <= 0)
			{
				//ReportObjectDealloc(static_cast<Class*>(this));
				delete this;
			}

			return aNumRefs;
		}

		//! \brief    Top-level implementation of IUnknown::QueryInterface.
		//! 
		//! \remarks
		//! This method implies that the implementation class provides
		//! interface discovery information via the \a InternalQueryInterface
		//! static member function.
		STDMETHODIMP QueryInterface(REFIID theIID, void ** theOut)
		{
			if (theOut == nullptr) return E_POINTER;

			if (theIID == __uuidof(IUnknown) || theIID == __uuidof(IObject) || theIID == _uuidof(Class))
				*theOut = reinterpret_cast<void*>(this);
			else
			{
				HRESULT aRes = DiscoverableBase::InternalQueryInterface(theIID, this, theOut);
				if (FAILED(aRes))
				{
					*theOut = nullptr;
					return aRes;
				}
			}

			AddRef();
			return S_OK;
		}

		DECLARE_DISAMBIGUATION_CAST(Class, DiscoverableBase);

		template<class T>
		static HRESULT InternalQueryInterface(REFIID theIID, T* obj, void** theOut)
		{
			if (theIID == _uuidof(Class))
			{
				*theOut = (void*)static_cast<const Class*>(obj);
				return S_OK;
			}

			return DiscoverableBase::InternalQueryInterface(theIID, obj, theOut);
		}

		virtual HRESULT __stdcall GetTypeId(IID* outType)
		{
			if (outType == nullptr) return E_POINTER;

			*outType = _uuidof(Class);
			return S_OK;
		}

		virtual void* __stdcall CastTo(REFIID riid) const
		{
			if (riid == _uuidof(IUnknown) || riid == _uuidof(IObject) || riid == _uuidof(Class))
				return const_cast<void*>(reinterpret_cast<const void*>(this));
			else
			{
				void* ret;
				HRESULT aRes = DiscoverableBase::InternalQueryInterface(riid, const_cast<ComObject*>(this), &ret);
				if (FAILED(aRes))
					return nullptr;
				return ret;
			}
		}

		virtual string ToString() const
		{
			return typeid(Class).name();
		}

		virtual HRESULT __stdcall ToString(IBlob** outValue) const
		{
			if (!outValue) return E_POINTER;

			*outValue = Help::String::AsBlob(ToString()).Detach();

			return S_OK;
		}

		template<class Q>
		const Q* CastTo() const { return reinterpret_cast<const Q*>(CastTo(__uuidof(Q))); }

		template<class Q>
		Q* CastTo() { return reinterpret_cast<Q*>(CastTo(__uuidof(Q))); }

		template<class Q>
		HRESULT QueryInterface(Q** pp)
		{
			return QueryInterface(_uuidof(Q), reinterpret_cast<void**>(pp));
		}

		template<class Q>
		HRESULT QueryInterface(CS::Details::StoredRef<com_ptr<Q>> pp)
		{
			return QueryInterface(_uuidof(Q), pp);
		}
	};

	/**
	 * \brief Base class for reference-counted types defined using Sharpish.
	 * 
	 * When declaring your own reference type, don't directly inherit from Object
	 * or any other reference type. Instead, inherit from ComObject and pass your
	 * base type as the appropriate template parameter.
	 * 
	 * \see CS::ComObject
	 */
	class __declspec(uuid("b7a9ea93-6b5e-4fde-9a6c-ea9872a00e7e"))
		Object : public ComObject<Object> { };

	}

//template<class T>
//__declspec(selectany) bool CS::Details::HasStaticInitializer<T>::isInitialized = [](void(*si)()) { CS::Details::RSI(si); return false; }(&T::StaticInit);
