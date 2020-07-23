#pragma once

// Macro to get the UUID of any type
#ifdef _uuidof
#undef _uuidof
#endif
#define _uuidof(T) (CS::Details::Identifier<T>::UUIDOf())

// Macro to get the UUID of a template declaration (will be the same for vector<int>, vector<char>, etc.)
#define _uuidof_generic(T) (CS::Details::Identifier<T>::GenericID())

// Binds a UUID to a type. This way even stuff like int and string can have IDs. 
// Must be used in global namespace
// T - the type to assign an ID to
// IDSTR - The UUID string, same as you'd pass to __declspec(uuid())
#define DECLARETYPEID(T, IDSTR) namespace CS { namespace Details { template class UUIDHolder<T>; template class __declspec(uuid(IDSTR)) UUIDHolder<T>; \
		template<> struct Identifier<T> : public DeclaredIdentifier<T> { }; } }

// Binds a UUID to a template type.
// Must be used withing the CS namespace.
// N - The number of template arguments the template type has
// TT - the template type
// IDSTR - The UUID string, same as you'd pass to __declspec(uuid())
#define DECLAREDETAILSGENERICID(N, TT, IDSTR) template class UUIDHolder##N<TT>; template class __declspec(uuid(IDSTR)) UUIDHolder##N<TT>; \
		template<__DECL(G,N)> struct Identifier<TT<__ARG(G,N)>> : \
		public GenericIdentifier<TT<__ARG(G,N)>, GenericInstanceID<&__uuidof(UUIDHolder##N<TT>),__ARG(G,N)>> { };

#define DECLAREGENERICID(N, TT, IDSTR) namespace CS { namespace Details { DECLAREDETAILSGENERICID(N, TT, IDSTR) } }

#define __ARG_FMT(x,N) x##N
#define __DECL_FMT(x,N) class x##N

#define __ARG(x,N) __CHAIN(x,N,__ARG)
#define __DECL(x,N) __CHAIN(x,N,__DECL)

// These macros assist in the declaration of a lot of redundant junk related to handling UUIDs of templates
#define __CHAIN_N_X(x,N,NM1,FMT) __CHAIN_##NM1 (x,FMT), FMT##_FMT (x,N)
#define __CHAIN_N(x,N,NM1,FMT) __CHAIN_N_X(x,N,NM1,FMT)
#define __CHAIN_1(x,FMT) FMT##_FMT (x,1)
#define __CHAIN_2(x,FMT) __CHAIN_1(x,FMT), FMT##_FMT (x,2)
#define __CHAIN_3(x,FMT) __CHAIN_2(x,FMT), FMT##_FMT (x,3)
#define __CHAIN_4(x,FMT) __CHAIN_3(x,FMT), FMT##_FMT (x,4)
#define __CHAIN_5(x,FMT) __CHAIN_4(x,FMT), FMT##_FMT (x,5)
#define __CHAIN_6(x,FMT) __CHAIN_5(x,FMT), FMT##_FMT (x,6)
#define __CHAIN_7(x,FMT) __CHAIN_6(x,FMT), FMT##_FMT (x,7)
#define __CHAIN_8(x,FMT) __CHAIN_7(x,FMT), FMT##_FMT (x,8)
#define __CHAIN_9(x,FMT) __CHAIN_8(x,FMT), FMT##_FMT (x,9)
#define __CHAIN_10(x,FMT) __CHAIN_9(x,FMT), FMT##_FMT (x,10)
#define __CHAIN_11(x,FMT) __CHAIN_10(x,FMT), FMT##_FMT (x,11)
#define __CHAIN_12(x,FMT) __CHAIN_11(x,FMT), FMT##_FMT (x,12)
#define __CHAIN_13(x,FMT) __CHAIN_12(x,FMT), FMT##_FMT (x,13)
#define __CHAIN_14(x,FMT) __CHAIN_13(x,FMT), FMT##_FMT (x,14)
#define __CHAIN_15(x,FMT) __CHAIN_14(x,FMT), FMT##_FMT (x,15)
#define __CHAIN_16(x,FMT) __CHAIN_15(x,FMT), FMT##_FMT (x,16)
#define __CHAIN_X(x,N,FMT) __CHAIN_##N (x,FMT)
#define __CHAIN(x,N,FMT) __CHAIN_X(x,N,FMT)

namespace CS { 
    template <typename Signature>
    class Delegate;

    namespace Details
    {
	    template<class T>
	    struct Identifier_Base
	    {	
		    //static REFIID GenericID() { return IID_NULL; }
	    };

	    template<class T>
	    struct Identifier : public Identifier_Base<T>
	    {
		    static REFIID UUIDOf() { return __uuidof(T); }
	    };

	    template<>
	    struct Identifier<void> : public Identifier_Base<void>
	    {
		    static REFIID UUIDOf() { return IID_NULL; }
	    };

	    // specialize for different forms and flavors that types come in
	    template<class T>
	    struct Identifier<T*> : public Identifier<T> { };

	    template<class T>
	    struct Identifier<T&> : public Identifier<T> { }; // might be bad

	    template<class T>
	    struct Identifier<const T&> : public Identifier<T> { }; // might be bad

	    // A dummy template that can have UUIDs attached to it.
	    template<class T> class UUIDHolder { };

    #define __TT_FMT(x,N) typename
    #define __TT(N) __CHAIN(0,N,__TT)
    #define __DECL_TPL_UUIDHOLDER(N) template<template<__TT(N)> class TT> class UUIDHolder##N { };

    // Declares a bunch of UUIDHolderX classes that all take template templates for arguments.
    __DECL_TPL_UUIDHOLDER(1)
    __DECL_TPL_UUIDHOLDER(2)
    __DECL_TPL_UUIDHOLDER(3)
    __DECL_TPL_UUIDHOLDER(4)
    __DECL_TPL_UUIDHOLDER(5)
    __DECL_TPL_UUIDHOLDER(6)
    __DECL_TPL_UUIDHOLDER(7)
    __DECL_TPL_UUIDHOLDER(8)
    __DECL_TPL_UUIDHOLDER(9)
    __DECL_TPL_UUIDHOLDER(10)
    __DECL_TPL_UUIDHOLDER(11)
    __DECL_TPL_UUIDHOLDER(12)
    __DECL_TPL_UUIDHOLDER(13)
    __DECL_TPL_UUIDHOLDER(14)
    __DECL_TPL_UUIDHOLDER(15)
    __DECL_TPL_UUIDHOLDER(16)

	    // Specializations of Identifier for primitives and value types
	    template<typename T>
	    struct DeclaredIdentifier : public Identifier_Base<T>
	    {
		    static REFIID UUIDOf() { return __uuidof(UUIDHolder<T>); }
	    };

	    static UUID Smash_UUID(REFIID a, REFIID b) 
	    {
		    UINT64 out[2];
		    UINT64* ap = (UINT64*)&a;
		    UINT64* bp = (UINT64*)&b;
		    out[0] = (bp[0] << 1) | (bp[0] >> 63);
		    out[1] = (bp[1] << 1) | (bp[1] >> 63);
		    out[0] = ap[0] ^ out[0];
		    out[1] = ap[1] ^ out[1];
		    return *(UUID*)out;
	    }

	    // GenericInstanceID provides implementation for Identifier specialized for templated types
	    template<LPCGUID ID, 
		    typename G1 = CNull, typename G2 = CNull, typename G3 = CNull, typename G4 = CNull, 
		    typename G5 = CNull, typename G6 = CNull, typename G7 = CNull, typename G8 = CNull, 
		    typename G9 = CNull, typename G10 = CNull, typename G11 = CNull, typename G12 = CNull, 
		    typename G13 = CNull, typename G14 = CNull, typename G15 = CNull, typename G16 = CNull>
	    struct GenericInstanceID
	    {
		    static constexpr const UUID& GenericID() { return *ID; }

		    static REFIID UUIDOf() 
		    {
			    static const UUID x = Smash_UUID(*ID, 
				    Smash_UUID(_uuidof(G1), 
				    Smash_UUID(_uuidof(G2), 
				    Smash_UUID(_uuidof(G3), 
				    Smash_UUID(_uuidof(G4), 
				    Smash_UUID(_uuidof(G5), 
				    Smash_UUID(_uuidof(G6), 
				    Smash_UUID(_uuidof(G7), 
				    Smash_UUID(_uuidof(G8), 
				    Smash_UUID(_uuidof(G9), 
				    Smash_UUID(_uuidof(G10), 
				    Smash_UUID(_uuidof(G11), 
				    Smash_UUID(_uuidof(G12), 
				    Smash_UUID(_uuidof(G13), 
				    Smash_UUID(_uuidof(G14), 
				    Smash_UUID(_uuidof(G15), 
					    _uuidof(G15)))))))))))))))));
			
			    return x; 
		    }
	    };

	    // Basic generic class type. T is the instanced type of the generic, and GenericInstance is GenericInstanceID with appropriate parameters
	    template<class T, class GenericInstance>
	    struct GenericIdentifier : public Identifier_Base<T>, public GenericInstance { };
    }
}