#pragma once

#include <vector>
#include "Make.h"
#include "ComClass.h"
#include "weak_ptr.h"

typedef unsigned __int32 FourCC;

namespace CS
{
	class ComTypeBase
	{
	};

#if SUPPORTED_CPP0X
	template<typename BaseType = IWeakReferenceSource2, typename... Implements>
	class ComType : public ComEntry<BaseType, Implements...>, public ComTypeBase
	{
		typedef ComDiscovery<BaseType, Implements...> DiscoverableBase;
	protected:
		typedef ComEntry<BaseType, Implements...> Base;
	public:
		typedef ComType<BaseType, Implements...> Discoverable;
#else
	template<typename BaseType = IWeakReferenceSource2, typename I1 = _Null, typename I2 = _Null, typename I3 = _Null, typename I4 = _Null, typename I5 = _Null>
	class ComType : public ComEntry<BaseType, I1, I2, I3, I4, I5>, public ComTypeBase
	{
		typedef ComDiscovery<BaseType, I1, I2, I3, I4, I5> DiscoverableBase;
	protected:
		typedef ComEntry<BaseType, I1, I2, I3, I4, I5> Base;
	public:
		typedef ComType<BaseType, I1, I2, I3, I4, I5> Discoverable;
#endif

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

#if (SUPPORTED_CPP0X != 0)

		template<typename tSomething, typename ... tParams>
		ComType(tSomething&& first, tParams && ... theParams)
			: Base(std::forward<tSomething>(first), std::forward<tParams>(theParams)...)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif

		{
			_weakRef->SetUnknown(reinterpret_cast<IUnknown*>(this));
		}

#else

		template<typename tParam1>
		ComType(tParam1 theParam1)
			: Base(theParam1)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{
			_weakRef->SetUnknown(reinterpret_cast<IUnknown*>(this));
		}

		template<typename tParam1, typename tParam2>
		ComType(tParam1 theParam1, tParam2 theParam2)
			: Base(theParam1, theParam2)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{
			_weakRef->SetUnknown(reinterpret_cast<IUnknown*>(this));
		}

		template<typename tParam1, typename tParam2, typename tParam3>
		ComType(tParam1 theParam1, tParam2 theParam2, tParam3 theParam3)
			: Base(theParam1, theParam2, theParam3)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{
			_weakRef->SetUnknown(reinterpret_cast<IUnknown*>(this));
		}

		template<typename tParam1, typename tParam2, typename tParam3, typename tParam4>
		ComType(tParam1 theParam1, tParam2 theParam2, tParam3 theParam3, tParam4 theParam4)
			: Base(theParam1, theParam2, theParam3, theParam4)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{
			_weakRef->SetUnknown(reinterpret_cast<IUnknown*>(this));
		}

		template<typename tParam1, typename tParam2, typename tParam3, typename tParam4, typename tParam5>
		ComType(tParam1 theParam1, tParam2 theParam2, tParam3 theParam3, tParam4 theParam4, tParam5 theParam5)
			: Base(theParam1, theParam2, theParam3, theParam4, theParam5)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{
			_weakRef->SetUnknown(reinterpret_cast<IUnknown*>(this));
		}

#endif	// (SUPPORTED_CPP0X != 0)


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
		std::vector<void(*)()>& RSI(void(*si)());

		template< void(*A)(), void(*B)() >
		struct SameFunc { static const bool value = false; };

		template<void(*A)()>
		struct SameFunc<A, A> { static const bool value = true; };

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
			static const bool value = SameFunc<&T::StaticInit, &T::Base::StaticInit>::value; // type::value;
			static bool isInitialized;
		};

	}

#if SUPPORTED_CPP0X

	template<typename Class, typename BaseClass = ComType<>, typename... Implements>
	class ComObject : public BaseClass, public Implements...
	{
		template<class T>
		friend class Details::HasStaticInitializer;

		typedef ComDiscovery<BaseClass, Implements...> DiscoverableBase;
	protected:
		typedef ComObject<Class, BaseClass, Implements...> Base;

#else

	template<typename Class, typename CBase = ComType<>, typename I1 = _Null, typename I2 = _Null, typename I3 = _Null, typename I4 = _Null, typename I5 = _Null>
	class ComObject : public ComEntry<CBase, I1, I2, I3, I4, I5>
	{
		typedef ComDiscovery<CBase, I1, I2, I3, I4, I5> DiscoverableBase;
		typedef ComEntry<CBase, I1, I2, I3, I4, I5> BaseClass;
	protected:
		typedef ComObject<Class, CBase, I1, I2, I3, I4, I5> Base;

#endif

	public:
		typedef Base Discoverable;
		typedef Class Interface;

#ifdef ENABLE_MEMORY_DEBUGGING
		bool DebugWatch;
#endif

		//static void StaticInit() { }

#if (SUPPORTED_CPP0X != 0)

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

#else

		ComObject()
#ifdef ENABLE_MEMORY_DEBUGGING
			: DebugWatch(false)
#endif
		{ }

		template<typename tParam1>
		ComObject(tParam1 theParam1)
			: BaseClass(theParam1)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{ }

		template<typename tParam1, typename tParam2>
		ComObject(tParam1 theParam1, tParam2 theParam2)
			: BaseClass(theParam1, theParam2)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{ }

		template<typename tParam1, typename tParam2, typename tParam3>
		ComObject(tParam1 theParam1, tParam2 theParam2, tParam3 theParam3)
			: BaseClass(theParam1, theParam2, theParam3)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{ }

		template<typename tParam1, typename tParam2, typename tParam3, typename tParam4>
		ComObject(tParam1 theParam1, tParam2 theParam2, tParam3 theParam3, tParam4 theParam4)
			: BaseClass(theParam1, theParam2, theParam3, theParam4)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{ }

		template<typename tParam1, typename tParam2, typename tParam3, typename tParam4, typename tParam5>
		ComObject(tParam1 theParam1, tParam2 theParam2, tParam3 theParam3, tParam4 theParam4, tParam5 theParam5)
			: BaseClass(theParam1, theParam2, theParam3, theParam4, theParam5)
#ifdef ENABLE_MEMORY_DEBUGGING
			, DebugWatch(false)
#endif
		{ }

#endif	// (SUPPORTED_CPP0X != 0)

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

	/// <summary> The base of most major classes in the EVE object model. </summary>
	/// <remarks>
	/// 		 Implements IUnknown, weak referencing, 
	/// 		 basic support for EVE's serialization mechanism.
	/// </remarks>
	class __declspec(uuid("b7a9ea93-6b5e-4fde-9a6c-ea9872a00e7e"))
		Object : public ComObject<Object>
	{
	public:
		PROPERTY_READONLY(FourCC, SerializationId);
		virtual FourCC GetSerializationId() { return 0; }
	};

	template<class T>
	FourCC GetSerialIDOfCommonType()
	{
		return ((T*)0)->T::GetSerializationId();
	}
	}

template<class T>
__declspec(selectany) bool CS::Details::HasStaticInitializer<T>::isInitialized = [](void(*si)()) { CS::Details::RSI(si); return false; }(&T::StaticInit);
