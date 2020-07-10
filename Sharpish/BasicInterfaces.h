#pragma once

#include <Unknwn.h>
#include <D3DCommon.h>

// Helpful macros and stuff that just make life easier.
#ifdef cominterface
#undef cominterface
#endif
#define cominterface struct __declspec(novtable)

#ifdef DECLARE_INTERFACE
#undef DECLARE_INTERFACE
#endif
#define DECLARE_INTERFACE(iface)                        cominterface iface

#ifdef DECLARE_INTERFACE_
#undef DECLARE_INTERFACE_
#endif
#define DECLARE_INTERFACE_(iface, baseiface)            cominterface iface : public baseiface

#ifdef DECLARE_INTERFACE_IID
#undef DECLARE_INTERFACE_IID
#endif
#define DECLARE_INTERFACE_IID(iface, iid)               cominterface DECLSPEC_UUID(iid) iface

#ifdef DECLARE_INTERFACE_IID_
#undef DECLARE_INTERFACE_IID_
#endif
#define DECLARE_INTERFACE_IID_(iface, baseiface, iid)   cominterface DECLSPEC_UUID(iid) iface : public baseiface

#define comid(ID) __declspec(uuid(ID))

#define METHOD virtual HRESULT __stdcall
#define METHOD_(Ret) virtual Ret __stdcall

extern "C++"
{
	namespace Ext
	{
		cominterface comid("00000037-0000-0000-C000-000000000046") IWeakReference
			: public IUnknown
		{
			METHOD Resolve(REFIID riid, void **objectReference) = 0;

			template <typename T>
			HRESULT Resolve(_Out_ T** objectReference)
				{ return Resolve(__uuidof(T), (void**)objectReference); }
		};

		cominterface comid("00000038-0000-0000-C000-000000000046") IWeakReferenceSource
			: public IUnknown
		{
			METHOD GetWeakReference(IWeakReference **weakReference) = 0;
		};
	}

	namespace CS
	{
		namespace Details
		{
			template<typename T>
			class StoredRef;
		}

		template<typename T>
		class com_ptr;

		typedef ID3DBlob IBlob;

		// IObject extends IUnknown with a couple methods that ordinarily don't make sense in
		// the classic COM model. EVE's runtime model has a few additional requirements that COM doesn't have:
		// - The class implementing IObject must have its own UUID that can be queried for. 
		// - All interfaces an object implements can be queried from any other cominterface it implements.
		//    There is never a need to chain calls to QueryInterface, though you may still want to QueryInterface(IUnknown)
		//    for object comparison purposes.
		// - All interfaces of an object share the same reference count.
		// - multiple inheritance of base classes is not allowed, and multiple implementations of the same 
		//    cominterface are not allowed.
		cominterface comid("428EDA6A-3C61-4FE9-AAE5-012C69672D38") IObject
			: public IUnknown
		{
			// CastTo is functionally similar to QueryInterface, 
			// with the exception that it does not change the reference count when it returns successfully.
			// Returns a pointer to the requested type on success, null on failure.
			// Because all interfaces on an object must share the same reference counted instance,
			// there is no need for casting to influence the reference count.
			METHOD_(void*) CastTo(REFIID riid) const = 0;

			// Gets the most-derived type ID of the object instance. This is not the same as _uuidof(),
			// which only gets the UUID of the explicitly declared type at compile time.
			// The type of the object is stored in the typeID parameter.
			// Returns E_POINTER if typeID is null.
			// Return E_FAIL if the object instance is not identifiable (i.e. classic COM).
			// Otherwise returns S_OK.
			METHOD GetTypeId(OUT IID* typeID) = 0;

			// Gets a string representation of the object.
			// This allows implementations to override string conversion for derived types.
			METHOD ToString(OUT IBlob** value) const = 0;

			template<class Q>
			const Q* CastTo() const { return reinterpret_cast<const Q*>(CastTo(__uuidof(Q))); }

			template<class Q>
			Q* CastTo() { return reinterpret_cast<Q*>(CastTo(__uuidof(Q))); }

			template<class Q>
			HRESULT QueryInterface(Q** pp)
			{
				return reinterpret_cast<IUnknown*>(this)->QueryInterface(_uuidof(Q), reinterpret_cast<void**>(pp));
			}

			template<class Q>
			HRESULT QueryInterface(CS::Details::StoredRef<com_ptr<Q>> pp)
			{
				return reinterpret_cast<IUnknown*>(this)->QueryInterface(_uuidof(Q), pp);
			}

			//string ToString() const;
		};

		cominterface comid("79051497-97DE-4D05-9C63-761223DAE84D") IWeakReferenceSource2
			: public IObject, public Ext::IWeakReferenceSource
		{ };
	}
}
