#pragma once

//#include "ObjectModelTraits.h"
#include "BasicInterfaces.h"

#ifndef _uuidof
#define _uuidof __uuidof
#endif

#define EVE_COM_IMPL

// Original source provided by http://www.codeproject.com/Articles/249257/Implementing-COM-Interfaces-with-Cplusplus0x-Varia

// Heavy modifications made by Evan Lang of Valorem, Inc. (elang@valorem.com)

/*!
*
* \section License
*
* Copyright (c) 2011, KO Software (official@ko-sw.com)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*     - All original and modified versions of this source code must include the
*       above copyright notice, this list of conditions and the following
*       disclaimer.
*
*     - The name of KO Software may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY KO SOFTWARE ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT WILL KO SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BasicInterfaces.h"
#include <utility>

////////////////////////////////////////////////////////////////////////////////

//! \def SUPPORTED_CPP0X
//! When set to nonzero, C++0X code with variadic templates will be present.

#if _MSC_VER >= 1800
#  define SUPPORTED_CPP0X 1
#else
#  define SUPPORTED_CPP0X 0
struct _Null { };
#endif //__GXX_EXPERIMENTAL_CXX0X__

////////////////////////////////////////////////////////////////////////////////

//! \def SUPPORTED_UUIDOF
//! When set to nonzero, assumes that the compiler can provide a built-in
//! __uuidof operator.

#ifdef _MSC_EXTENSIONS
#  define SUPPORTED_UUIDOF 1
#else
#  define SUPPORTED_UUIDOF 0
#endif //_MSC_EXTENSIONS

////////////////////////////////////////////////////////////////////////////////

#if (SUPPORTED_UUIDOF == 0)

template<typename t>
struct hold_uuidof { static GUID __IID; };

#  define DECLSPEC_NOVTABLE

#  define DEFINE_UUIDOF_ID(Q, IID) template<> GUID hold_uuidof<Q>::__IID = IID;
#  define DEFINE_UUIDOF(Q) DEFINE_UUIDOF_ID(Q, IID_##Q)
#  define __uuidof(Q) hold_uuidof<Q>::__IID

#endif	// (SUPPORTED_UUIDOF != 0)

////////////////////////////////////
// Define constructor-forwarding macro
////////////////////////////////////
#define FWD_CONSTRUCTOR(Class, Base, ...) \
	template<typename ... tParams> \
	Class(tParams && ... theParams) \
	: Base( std::forward<tParams>(theParams)... ), ## __VA_ARGS__ \
	{ }

template<typename To, typename From, typename Interface, typename Base,
	bool ToSame = std::is_same<To, Interface>::value,
	bool FromSame = std::is_same<From, Interface>::value>
	struct __StandardDisambiguation;

template<typename To, typename From, typename Interface, typename Base>
struct __StandardDisambiguation<To, From, Interface, Base, true, false>
{
	static To* Cast(From* obj) { return static_cast<To*>(obj); }
};

template<typename To, typename From, typename Interface, typename Base>
struct __StandardDisambiguation<To, From, Interface, Base, true, true>
{
	static To* Cast(From* obj) { return static_cast<To*>(obj); }
};

template<typename To, typename From, typename Interface, typename Base>
struct __StandardDisambiguation<To, From, Interface, Base, false, false>
{
	static To* Cast(From* obj)
	{
		return _ComTraits<Base>::Discoverable::DisambiguationCast<To, Interface>(const_cast<Interface*>(static_cast<const Interface*>(obj)));
	}
};

#define DECLARE_DISAMBIGUATION_CAST(Interface, Base) \
	template<typename To> \
	struct IsCastable { static const bool Yes = std::is_base_of<To, Interface>::value; }; \
	template<typename To, typename From> \
	static To* DisambiguationCast(From* obj) { return __StandardDisambiguation<To, From, Interface, Base>::Cast(obj); }

#define DECLARE_DISAMBIGUATION_CAST_INSTANCE(Discoverable) \
		template<typename To, typename From> \
		static To* DisambiguationCast(From* obj) { return Discoverable::DisambiguationCast<To, From>(obj); }

template<typename I>
class DECLSPEC_NOVTABLE ComDiscovery1
{
public:
	typedef I Interface;
	typedef ComDiscovery1<I> Discoverable;

	template<typename To>
	struct IsCastable
	{
		static const bool Yes = std::is_base_of<To, Interface>::value;
	};

	template<typename To, typename From>
	static To* DisambiguationCast(From* obj)
	{
		return static_cast<To*>(obj);
	}

	template<typename tObject>
	static HRESULT InternalQueryInterface(REFIID theIID, tObject * theObj, void ** theOut)
	{
		if (theIID == _uuidof(I))
			*theOut = _ComTraits<tObject>::Discoverable::DisambiguationCast<I>(theObj);
		else
			return E_NOINTERFACE;

		return S_OK;
	}

	template<typename tObject>
	static void InternalCast(REFIID id, tObject* obj, void*& out)
	{
		if (id == _uuidof(I))
			out = _ComTraits<tObject>::Discoverable::DisambiguationCast<I>(theObj);
	}
};

template<typename To, typename From, typename IA, typename IB,
	bool ACastable = IA::IsCastable<To>::Yes,
	bool BCastable = IB::IsCastable<To>::Yes,
	bool ASame = std::is_same<typename IA::Interface, From>::value>
	struct __CompoundDisambiguation;

template<typename To, typename From, typename IA, typename IB>
struct __CompoundDisambiguation<To, From, IA, IB, false, false, false>
{
	static To* Cast(From* obj)
	{
		static_assert(false, "Invalid cast!");
		return nullptr;
	};
};

template<typename To, typename From, typename IA, typename IB, bool Same>
struct __CompoundDisambiguation<To, From, IA, IB, true, false, Same>
{
	static To* Cast(From* obj)
	{
		return IA::DisambiguationCast<To>(obj);
	};
};

template<typename To, typename From, typename IA, typename IB, bool Same>
struct __CompoundDisambiguation<To, From, IA, IB, false, true, Same>
{
	static To* Cast(From* obj)
	{
		return IB::DisambiguationCast<To, From>(obj);
	};
};

template<typename To, typename From, typename IA, typename IB>
struct __CompoundDisambiguation<To, From, IA, IB, true, true, false>
{
	static To* Cast(From* obj)
	{
		return IA::DisambiguationCast<To, From>(obj);
	};
};

template<typename To, typename From, typename IA, typename IB>
struct __CompoundDisambiguation<To, From, IA, IB, true, true, true>
{
	static To* Cast(From* obj)
	{
		return IB::DisambiguationCast<To, From>(obj);
	};
};

template<typename IA, typename IB>
class DECLSPEC_NOVTABLE ComDiscoveryCompound : public IA, public IB
{
public:
	typedef ComDiscoveryCompound<IA, IB> Discoverable;
	typedef void Interface;

	template<typename T>
	struct IsCastable
	{
		static const bool Yes = IA::IsCastable<T>::Yes || IB::IsCastable<T>::Yes;
	};

#if SUPPORTED_CPP0X

	template<class To, class From, typename A = typename IA::IsCastable<To>, typename B = typename IB::IsCastable<To>>
	static typename std::enable_if<
		A::Yes && !B::Yes,
		To*>::type DisambiguationCast(From* obj)
	{
		return IA::DisambiguationCast<To>(obj);
	}

	template<class To, class From, typename A = typename IA::IsCastable<To>, typename B = typename IB::IsCastable<To>>
	static typename std::enable_if<
		!A::Yes && B::Yes,
		To*>::type DisambiguationCast(From* obj)
	{
		return IB::DisambiguationCast<To>(obj);
	}

	template<class To, class From, typename A = typename IA::IsCastable<To>, typename B = typename IB::IsCastable<To>>
	static typename std::enable_if<
		!A::Yes && !B::Yes,
		To*>::type DisambiguationCast(From* obj)
	{
		static_assert(false, "Invalid cast!");
		return nullptr;
	}

	template<class To, class From, typename A = typename IA::IsCastable<To>, typename B = typename IB::IsCastable<To>>
	static typename std::enable_if<
		A::Yes && B::Yes &&
		!std::is_same<typename IA::Interface, From>::value,
		To*>::type DisambiguationCast(From* obj)
	{
		return IA::DisambiguationCast<To, From>(obj);
	}

	template<class To, class From, typename A = typename IA::IsCastable<To>, typename B = typename IB::IsCastable<To>>
	static typename std::enable_if<
		A::Yes && B::Yes &&
		std::is_same<typename IA::Interface, From>::value,
		To*>::type DisambiguationCast(From* obj)
	{
		return IB::DisambiguationCast<To, From>(obj);
	}
#else
private:

public:
	template<class To, class From>
	static To* DisambiguationCast(From* obj)
	{
		return __CompoundDisambiguation<To, From, IA, IB>::Cast(obj);
	}

#endif

	template<typename tObject>
	static HRESULT InternalQueryInterface(REFIID theIID, tObject * theObj, void ** theOut)
	{
		HRESULT aRes = IA::InternalQueryInterface(theIID, theObj, theOut);

		if (FAILED(aRes))
			aRes = IB::InternalQueryInterface(theIID, theObj, theOut);

		return aRes;
	}

	template<typename tObject>
	static void InternalCast(REFIID id, tObject* obj, void*& out)
	{
		IA::InternalCast(id, obj, out);

		if (!out)
			IB::InternalCast(id, obj, out);
	}
};

template<typename T, typename _N1 = void>
struct _ComTraits;

#define MakeInterfaceTransparent(Interface, ...) \
template<> \
struct _ComTraits<Interface, void> \
{ \
	typedef ComDiscovery<ComDiscovery1<Interface>, __VA_ARGS__> Discoverable; \
};

//! \brief
//! Provides discovery information about any number of COM interfaces.
//! 
//! \tparam I0_N    The list of interfaces being implemented.
template<typename ... I0_N>
class DECLSPEC_NOVTABLE ComDiscovery;


template<typename I0, typename ... I1_N>
class DECLSPEC_NOVTABLE ComDiscovery<I0, I1_N...> :
	public ComDiscoveryCompound
	<
	typename _ComTraits<I0>::Discoverable,
	ComDiscovery<I1_N...>
	> { };

template <typename I0>
class DECLSPEC_NOVTABLE ComDiscovery<I0> : public _ComTraits<I0>::Discoverable { };


template<typename T, typename _N1>
struct _ComTraits
{
	typedef ComDiscovery1<T> Discoverable;
	template<typename To, typename From>
	static To* DisambiguationCast(From* obj) { return static_cast<To*>(obj); }
};

template<class T>
struct _derp_void_alias
{
	typedef void type;
};

template<class T>
struct _ComTraits<T, typename _derp_void_alias<typename T::Discoverable>::type>
{
	typedef typename T::Discoverable Discoverable;

	template<typename To, typename From>
	static To* DisambiguationCast(From* obj) { return T::DisambiguationCast<To>(obj); }
};

////////////////////////////////////////////////////////////////////////////////

// Variadic-template versions

template<typename ... I0_N>
class DECLSPEC_NOVTABLE ComEntry;


template<typename I0, typename ... I1_N>
class DECLSPEC_NOVTABLE ComEntry<I0, I1_N...> : public I0, public I1_N...
{
public:
	FWD_CONSTRUCTOR(ComEntry, I0);
};

template <typename I0>
class DECLSPEC_NOVTABLE ComEntry<I0> : public I0
{
public:
	FWD_CONSTRUCTOR(ComEntry, I0);
};



////////////////////////////////////////////////////////////////////////////////

template<typename BaseType = IObject, typename... Implements>
class ComInstance : public ComEntry<BaseType, Implements...>
{
	typedef ComDiscovery<BaseType, Implements...> DiscoverableBase;
protected:
	typedef ComEntry<BaseType, Implements...> Base;
public:
	typedef ComInstance<BaseType, Implements...> Discoverable;

protected:
	ULONG _refCount;

public:
	typedef void Interface;

	//ComInstance() : _refCount(1) { }

	virtual ~ComInstance() { }

	FWD_CONSTRUCTOR(ComInstance, Base, _refCount(1))

		//#if (SUPPORTED_CPP0X != 0)
		//
		//	template<typename tSomething, typename ... tParams>
		//	ComInstance(tSomething&& first, tParams && ... theParams)
		//		: Base( std::forward<tSomething>(first), std::forward<tParams>(theParams)... ), _refCount(1)
		//	{ }
		//	
		//#else
		//	
		//	ComInstance() : _refCount(1)
		//	{ }
		//
		//	template<typename tParam1>
		//	ComInstance(tParam1 theParam1)
		//		: Base(theParam1), _refCount(1)
		//	{ }
		//
		//	template<typename tParam1, typename tParam2>
		//	ComInstance(tParam1 theParam1, tParam2 theParam2)
		//		: Base(theParam1, theParam2), _refCount(1)
		//	{ }
		//
		//	template<typename tParam1, typename tParam2, typename tParam3>
		//	ComInstance(tParam1 theParam1, tParam2 theParam2, tParam3 theParam3)
		//		: Base(theParam1, theParam2, theParam3), _refCount(1)
		//	{ }
		//
		//	template<typename tParam1, typename tParam2, typename tParam3, typename tParam4>
		//	ComInstance(tParam1 theParam1, tParam2 theParam2, tParam3 theParam3, tParam4 theParam4)
		//		: Base(theParam1, theParam2, theParam3, theParam4), _refCount(1)
		//	{ }
		//
		//	template<typename tParam1, typename tParam2, typename tParam3, typename tParam4, typename tParam5>
		//	ComInstance(tParam1 theParam1, tParam2 theParam2, tParam3 theParam3, tParam4 theParam4, tParam5 theParam5)
		//		: Base(theParam1, theParam2, theParam3, theParam4, theParam5), _refCount(1)
		//	{ }
		//	
		//#endif	// (SUPPORTED_CPP0X != 0)


public:

	ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&_refCount);
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG aNumRefs;

		aNumRefs = InterlockedDecrement(&_refCount);

		if (aNumRefs <= 0)
			delete this;

		return aNumRefs;
	}

	template<class T>
	static HRESULT InternalQueryInterface(REFIID theIID, T* obj, void** theOut)
	{
		return DiscoverableBase::InternalQueryInterface(theIID, obj, theOut);
	}

	DECLARE_DISAMBIGUATION_CAST_INSTANCE(DiscoverableBase)

	virtual void* __stdcall CastTo(REFIID riid) const
	{
		if (__uuidof(IUnknown) == riid || __uuidof(CS::IObject) == riid)
			return (void*)this;
		else
		{
			void* ret;
			HRESULT aRes = DiscoverableBase::InternalQueryInterface(riid, const_cast<ComInstance*>(this), &ret);
			if (FAILED(aRes))
				return nullptr;
			return ret;
		}
	}

	template<class Q>
	Q* CastTo() { return reinterpret_cast<Q*>(CastTo(__uuidof(Q))); }

	//! \brief    Top-level implementation of IUnknown::QueryInterface.
	//! 
	//! \remarks
	//! This method implies that the implementation class provides
	//! interface discovery information via the \a InternalQueryInterface
	//! static member function.
	STDMETHODIMP QueryInterface(REFIID theIID, void ** theOut)
	{
		if (theOut == nullptr) return E_POINTER;

		if (theIID == __uuidof(IUnknown) || theIID == __uuidof(CS::IObject))
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

typedef ComInstance<CS::IObject> ComBase;

template<typename Class, typename BaseClass = ComBase, typename... Implements>
class ComClass : public BaseClass, public Implements...
{
	typedef ComDiscovery<BaseClass, Implements...> DiscoverableBase;

protected:
	typedef ComClass<Class, BaseClass, Implements...> Base;

public:
	typedef Base Discoverable;
	typedef Class Interface;

	FWD_CONSTRUCTOR(ComClass, BaseClass)

public:

	ULONG STDMETHODCALLTYPE AddRef()
	{
		return BaseClass::AddRef();
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		return BaseClass::Release();
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

		if (theIID == __uuidof(IUnknown) || theIID == __uuidof(IObject) || theIID == __uuidof(Class))
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

	DECLARE_DISAMBIGUATION_CAST(Class, DiscoverableBase)

	template<class T>
	static HRESULT InternalQueryInterface(REFIID theIID, T* obj, void** theOut)
	{
		if (theIID == __uuidof(Class))
		{
			*theOut = const_cast<Class*>(static_cast<const Class*>(obj));
			return S_OK;
		}

		return DiscoverableBase::InternalQueryInterface(theIID, obj, theOut);
	}

	virtual void* __stdcall CastTo(REFIID riid) const
	{
		if (riid == __uuidof(IUnknown) || riid == __uuidof(IObject) || riid == __uuidof(Class))
			return const_cast<void*>(reinterpret_cast<const void*>(this));
		else
		{
			void* ret;
			HRESULT aRes = DiscoverableBase::InternalQueryInterface(riid, this, &ret);
			if (FAILED(aRes))
				return nullptr;
			return ret;
		}
	}
};

MakeInterfaceTransparent(CS::IWeakReferenceSource2, Ext::IWeakReferenceSource, CS::IObject)
