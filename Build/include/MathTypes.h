#pragma once

// Library of common math types used by EVE and EVE-based applications.
// Types include:
//
// Floating-point vectors: 
//     Float2, Float3, Float4
//     Float2A, Float3A, Float4A  (16-byte aligned)
//
// Integer Vectors:
//     Int2, Int3, Int4
//     Int2A, Int3A, Int4A  (16-byte aligned)
//
// Boolean Vectors:
//     Bool2, Bool3, Bool4
//     Bool2A, Bool3A, Bool4A  (!6-byte aligned)
//
// Matrices:
//     Float3x3, Float4x3, Float4x4
//     Float4x3A, Float4x4A  (16-byte aligned)
//
// The library is modeled after Unity and WPF's, and is built on top of the XM** APIs provided by DirectX.
// Vector types are templatized as well, so Float3 == Vector<float,3> and Float2A == Vector<float,2,true>

#include <DirectXMath.h>

namespace CS
{
	using namespace DirectX;

	struct Float4x4A;
	struct Float4x4;
	struct Float4x3A;
	struct Float4x3;
	struct Quaternion;
	struct QuaternionA;

	template<typename T, int Rank, bool Align = false>
	struct Vector;

	template<typename T, int Rank>
	struct VectorBaseTraits
	{
		typedef Vector<T, Rank, true> AlignedType;
		typedef Vector<T, Rank, false> UnalignedType;
		typedef T ElementType;
		static const int TypeRank = Rank;
	};

	namespace Details
	{
		template<typename T>
		struct VectorAccess;

		inline XMVECTOR __vectorcall Replicate(int x) { return XMVectorReplicateInt((uint32_t)x); }
		inline XMVECTOR __vectorcall Replicate(bool x) { return XMVectorReplicateInt(x ? 0xFFFFFFFF : 0); }
		inline XMVECTOR __vectorcall Replicate(float x) { return XMVectorReplicate(x); }

		template<>
		struct VectorAccess<int>
		{
			static inline int __vectorcall GetX(XMVECTOR v) { return XMVectorGetIntX(v); }
			static inline int __vectorcall GetY(XMVECTOR v) { return XMVectorGetIntY(v); }
			static inline int __vectorcall GetZ(XMVECTOR v) { return XMVectorGetIntZ(v); }
			static inline int __vectorcall GetW(XMVECTOR v) { return XMVectorGetIntW(v); }

			static inline void __vectorcall SetX(XMVECTOR& v, int x) { v = XMVectorSetIntX(v, x); }
			static inline void __vectorcall SetY(XMVECTOR& v, int x) { v = XMVectorSetIntY(v, x); }
			static inline void __vectorcall SetZ(XMVECTOR& v, int x) { v = XMVectorSetIntZ(v, x); }
			static inline void __vectorcall SetW(XMVECTOR& v, int x) { v = XMVectorSetIntW(v, x); }
		};

		template<>
		struct VectorAccess<float>
		{
			static inline float __vectorcall GetX(XMVECTOR v) { return XMVectorGetX(v); }
			static inline float __vectorcall GetY(XMVECTOR v) { return XMVectorGetY(v); }
			static inline float __vectorcall GetZ(XMVECTOR v) { return XMVectorGetZ(v); }
			static inline float __vectorcall GetW(XMVECTOR v) { return XMVectorGetW(v); }

			static inline void __vectorcall SetX(XMVECTOR& v, float x) { v = XMVectorSetX(v, x); }
			static inline void __vectorcall SetY(XMVECTOR& v, float x) { v = XMVectorSetY(v, x); }
			static inline void __vectorcall SetZ(XMVECTOR& v, float x) { v = XMVectorSetZ(v, x); }
			static inline void __vectorcall SetW(XMVECTOR& v, float x) { v = XMVectorSetW(v, x); }
		};

		template<>
		struct VectorAccess<bool>
		{
			static inline bool __vectorcall GetX(XMVECTOR v) { return 0 != XMVectorGetIntX(v); }
			static inline bool __vectorcall GetY(XMVECTOR v) { return 0 != XMVectorGetIntY(v); }
			static inline bool __vectorcall GetZ(XMVECTOR v) { return 0 != XMVectorGetIntZ(v); }
			static inline bool __vectorcall GetW(XMVECTOR v) { return 0 != XMVectorGetIntW(v); }

			static inline void __vectorcall SetX(XMVECTOR& v, bool x) { v = XMVectorSetIntX(v, x ? 0xFFFFFFFF : 0); }
			static inline void __vectorcall SetY(XMVECTOR& v, bool x) { v = XMVectorSetIntY(v, x ? 0xFFFFFFFF : 0); }
			static inline void __vectorcall SetZ(XMVECTOR& v, bool x) { v = XMVectorSetIntZ(v, x ? 0xFFFFFFFF : 0); }
			static inline void __vectorcall SetW(XMVECTOR& v, bool x) { v = XMVectorSetIntW(v, x ? 0xFFFFFFFF : 0); }
		};

		template<typename T>
		struct AlignedVectorComponentRef;

		template<>
		struct AlignedVectorComponentRef<float>
		{
			XMVECTOR& _x;
			int _index;
			inline AlignedVectorComponentRef(XMVECTOR& x, int index) : _x(x), _index(index) { }

			inline operator float() { return XMVectorGetByIndex(_x, _index); }
			inline float operator =(float v) { _x = XMVectorSetByIndex(_x, v, _index); return v; }
		};

		template<>
		struct AlignedVectorComponentRef<int>
		{
			XMVECTOR& _x;
			int _index;
			inline AlignedVectorComponentRef(XMVECTOR& x, int index) : _x(x), _index(index) { }

			inline operator int() { return (int)XMVectorGetIntByIndex(_x, _index); }
			inline int operator =(int v) { _x = XMVectorSetIntByIndex(_x, (uint32_t)v, _index); return v; }
		};

		template<>
		struct AlignedVectorComponentRef<bool>
		{
			XMVECTOR& _x;
			int _index;
			inline AlignedVectorComponentRef(XMVECTOR& x, int index) : _x(x), _index(index) { }

			inline operator bool() { return 0 != XMVectorGetIntByIndex(_x, _index); }
			inline bool operator =(bool v) { _x = XMVectorSetIntByIndex(_x, v ? 0xFFFFFFFF : 0, _index); return v; }
		};
	}

#define VectorSwizzleAligned(T,Rank) \
			template<int c0, int c1> inline Vector<T, 2, true> __vectorcall Swizzle2() \
			{ static_assert(c0 < Rank, "c0 template parameter out of range"); \
				static_assert(c1 < Rank, "c1 template parameter out of range"); \
				return XMVectorSwizzle<c0, c1, 0, 0>(_xm);} \
			template<int c0, int c1, int c2> inline Vector<T, 3, true> __vectorcall Swizzle3()\
			{ static_assert(c0 < Rank, "c0 template parameter out of range"); \
				static_assert(c1 < Rank, "c1 template parameter out of range"); \
				static_assert(c2 < Rank, "c2 template parameter out of range"); \
				return XMVectorSwizzle<c0, c1, c2, 0>(_xm); } \
			template<int c0, int c1, int c2, int c3> inline Vector<T, 4, true> __vectorcall Swizzle4() \
			{ static_assert(c0 < Rank, "c0 template parameter out of range"); \
				static_assert(c1 < Rank, "c1 template parameter out of range"); \
				static_assert(c2 < Rank, "c2 template parameter out of range"); \
				static_assert(c3 < Rank, "c3 template parameter out of range"); \
				return XMVectorSwizzle<c0, c1, c2, c3>(_xm); }

#define Vector2DataAligned(T) \
		protected: XMVECTOR _xm; \
		public: \
			operator XMVECTOR() const { return _xm; } PROPERTY(T, X); PROPERTY(T, Y); \
			inline T __vectorcall GetX() const { return Details::VectorAccess<T>::GetX(_xm); } \
			inline void __vectorcall SetX(T value) { Details::VectorAccess<T>::SetX(_xm, value); } \
			inline T __vectorcall GetY() const { return Details::VectorAccess<T>::GetY(_xm); } \
			inline void __vectorcall SetY(T value) { Details::VectorAccess<T>::SetY(_xm, value); } \
			inline AlignedType __vectorcall SplatX() const { return XMVectorSplatX(_xm); } \
			inline AlignedType __vectorcall SplatY() const { return XMVectorSplatY(_xm); } \
			inline Details::AlignedVectorComponentRef<T> operator[](int index) \
				{ return Details::AlignedVectorComponentRef<T>(_xm, index); }

#define Vector3DataAligned(T) \
		Vector2DataAligned(T) \
		PROPERTY(T, Z); PROPERTY(AsIs(Vector<T,2,true>&), XY); \
		inline T __vectorcall GetZ() const { return Details::VectorAccess<T>::GetZ(_xm); } \
		inline void __vectorcall SetZ(T value) { Details::VectorAccess<T>::SetZ(_xm, value); } \
		inline AlignedType __vectorcall SplatZ() const { return XMVectorSplatZ(_xm); } \
		inline Vector<T,2,true>& GetXY() { return *reinterpret_cast<Vector<T,2,true>*>(this); } \
		inline const Vector<T,2,true>& GetXY() const { return *reinterpret_cast<const Vector<T,2,true>*>(this); } \
		inline void SetXY(const Vector<T, 2, true>& v) { static auto control = XMVectorSetInt(0,0,0xFFFFFFFF,0xFFFFFFFF); _xm = XMVectorSelect(v,_xm,control); }

#define Vector4DataAligned(T) \
		Vector3DataAligned(T) \
		PROPERTY(T, W); PROPERTY(AsIs(Vector<T,3,true>&), XYZ); \
		inline T __vectorcall GetW() const { return Details::VectorAccess<T>::GetW(_xm); } \
		inline void __vectorcall SetW(T value) { Details::VectorAccess<T>::SetW(_xm, value); } \
		inline AlignedType __vectorcall SplatW() const { return XMVectorSplatW(_xm); } \
		inline Vector<T, 3, true>& GetXYZ() { return *reinterpret_cast<Vector<T, 3, true>*>(this); } \
		inline const Vector<T, 3, true>& GetXYZ() const { return *reinterpret_cast<const Vector<T, 3, true>*>(this); } \
		inline void SetXYZ(const Vector<T, 3, true>& v) { static auto control = XMVectorSetInt(0, 0, 0, 0xFFFFFFFF); _xm = XMVectorSelect(v, _xm, control); }

#define Vector4Members(T) \
		template<int shiftCount> inline AlignedType __vectorcall ShiftLeft(const AlignedType& rhs) const \
			{ return XMVectorShiftLeft<shiftCount>(*this, rhs); } \
		inline AlignedType __vectorcall ShiftLeft(const AlignedType& rhs, int shiftCount) const \
			{ return XMVectorShiftLeft(*this, rhs, (uint32_t)shiftCount); } \
		template<int count> inline AlignedType __vectorcall RotateLeft() const \
			{ return XMVectorRotateLeft<(uint32_t)count>(*this); } \
		inline AlignedType __vectorcall RotateLeft(int count) const \
			{ return XMVectorRotateLeft(*this, (uint32_t)count); } \
		template<int count> inline AlignedType __vectorcall RotateRight() const \
			{ return XMVectorRotateRight<(uint32_t)count>(*this); } \
		inline AlignedType __vectorcall RotateRight(int count) const \
			{ return XMVectorRotateRight(*this, (uint32_t)count); }

#define VectorSwizzleUnaligned(T,Rank) \
		template<int c0> inline Vector<T, 1, true> __vectorcall Swizzle1() const \
			{ static_assert(c0 < Rank, "c0 template parameter out of range"); \
			auto aref = (T*)this; \
			return XMVectorSet(aref[c0], 0, 0, 0); } \
		template<int c0, int c1> inline Vector<T, 2, true> __vectorcall Swizzle2() const \
			{ static_assert(c0 < Rank, "c0 template parameter out of range"); \
			static_assert(c1 < Rank, "c1 template parameter out of range"); \
			auto aref = (T*)this; \
			return XMVectorSet(aref[c0], aref[c1], 0, 0); } \
		template<int c0, int c1, int c2> inline Vector<T, 3, true> __vectorcall Swizzle3() const \
			{ static_assert(c0 < Rank, "c0 template parameter out of range"); \
			static_assert(c1 < Rank, "c1 template parameter out of range"); \
			static_assert(c2 < Rank, "c2 template parameter out of range"); \
			auto aref = (T*)this; \
			return XMVectorSet(aref[c0], aref[c1], aref[c2], 0); } \
		template<int c0, int c1, int c2, int c3> inline Vector<T, 4, true> __vectorcall Swizzle4() const \
			{ static_assert(c0 < Rank, "c0 template parameter out of range"); \
			static_assert(c1 < Rank, "c1 template parameter out of range"); \
			static_assert(c2 < Rank, "c2 template parameter out of range"); \
			static_assert(c3 < Rank, "c3 template parameter out of range"); \
			auto aref = (T*)this; \
			return XMVectorSet(aref[c0], aref[c1], aref[c2], aref[c3]); }

#define Vector2DataUnaligned(T) \
		T X, Y; \
		inline AlignedType __vectorcall SplatX() const { return Details::Replicate(X); } \
		inline AlignedType __vectorcall SplatY() const { return Details::Replicate(Y); } \
		inline T& operator[](int index) { return ((T*)this)[index]; }


#define Vector3DataUnaligned(T) \
		Vector2DataUnaligned(T) \
		T Z; \
		PROPERTY(AsIs(Vector<T,2,false>&), XY); \
		inline Vector<T,2,false>& GetXY() { return *reinterpret_cast<Vector<T,2,false>*>(this); } \
		inline const Vector<T,2,false>& GetXY() const { return *reinterpret_cast<const Vector<T,2,false>*>(this); } \
		inline void SetXY(const Vector<T,2,false>& v) { X = v.X; Y = v.Y; } \
		inline AlignedType __vectorcall SplatZ() const { return Details::Replicate(Z); }

#define Vector4DataUnaligned(T) \
		Vector3DataUnaligned(T) \
		T W; \
		PROPERTY(AsIs(Vector<T,3,false>&), XYZ); \
		inline Vector<T,3,false>& GetXYZ() { return *reinterpret_cast<Vector<T,3,false>*>(this); } \
		inline const Vector<T,3,false>& GetXYZ() const { return *reinterpret_cast<const Vector<T,3,false>*>(this); } \
		inline void SetXYZ(const Vector<T,3,false>& v) { X = v.X; Y = v.Y; Z = v.Z; } \
		inline AlignedType __vectorcall SplatW() const { return Details::Replicate(W); }


#define VectorFloat2Members \
			inline bool __vectorcall operator ==(const AlignedType& rhs) const { return XMVector2Equal(*this, rhs); } \
			inline bool __vectorcall operator !=(const AlignedType& rhs) const { return XMVector2NotEqual(*this, rhs); } \
			inline bool __vectorcall operator >(const AlignedType& rhs) const { return XMVector2Greater(*this, rhs); } \
			inline bool __vectorcall operator >=(const AlignedType& rhs) const { return XMVector2GreaterOrEqual(*this, rhs); } \
			inline bool __vectorcall operator <(const AlignedType& rhs) const { return XMVector2Less(*this, rhs); } \
			inline bool __vectorcall operator <=(const AlignedType& rhs) const { return XMVector2LessOrEqual(*this, rhs); } \
			inline bool __vectorcall NearEqual(const AlignedType& rhs, const AlignedType& epsilon) const { return XMVector2NearEqual(*this, rhs, epsilon); } \
			inline bool __vectorcall InBounds(const AlignedType& bounds) const { return XMVector2InBounds(*this, bounds); } \
			PROPERTY_READONLY(bool, IsNan); \
			inline bool __vectorcall GetIsNan() const { return XMVector2IsNaN(*this); } \
			PROPERTY_READONLY(bool, IsInfinite); \
			inline bool __vectorcall GetIsInfinite() const { return XMVector2IsInfinite(*this); } \
			inline float __vectorcall Dot(const AlignedType& rhs) const { return XMVectorGetX(XMVector2Dot(*this, rhs)); } \
			inline AlignedType __vectorcall SplatDot(const AlignedType& rhs) const { return XMVector2Dot(*this, rhs); } \
			inline float __vectorcall Cross(const AlignedType& rhs) const { return XMVectorGetX(XMVector2Cross(*this, rhs)); } \
			inline AlignedType __vectorcall SplatCross(const AlignedType& rhs) const { return XMVector2Cross(*this, rhs); } \
			PROPERTY_READONLY(float, Length); \
			inline float __vectorcall GetLength() const { return XMVectorGetX(XMVector2Length(*this)); } \
			inline AlignedType __vectorcall SplatLength() const { return XMVector2Length(*this); } \
			PROPERTY_READONLY(float, LengthSquared); \
			inline float __vectorcall GetLengthSquared() const { return XMVectorGetX(XMVector2LengthSq(*this)); } \
			inline AlignedType __vectorcall SplatLengthSquared() const { return XMVector2LengthSq(*this); } \
			PROPERTY_READONLY(float, ReciprocalLength); \
			inline float __vectorcall GetReciprocalLength() const { return XMVectorGetX(XMVector2ReciprocalLength(*this)); } \
			inline AlignedType __vectorcall SplatReciprocalLength() const { return XMVector2ReciprocalLength(*this); } \
			inline AlignedType __vectorcall Normalize() const { return XMVector2Normalize(*this); } \
			inline AlignedType __vectorcall ClampLength(float minLength, float maxLength) const \
				{ return XMVector2ClampLength(*this, minLength, maxLength); } \
			inline AlignedType __vectorcall ClampLengthV(const AlignedType& minLength, const AlignedType& maxLength) const \
				{ return XMVector2ClampLengthV(*this, minLength, maxLength); } \
			inline AlignedType __vectorcall Reflect(const AlignedType& normal) const \
				{ return XMVector2Reflect(*this, normal); } \
			inline AlignedType __vectorcall Refract(const AlignedType& normal, float refractionIndex) const \
				{ return XMVector2Refract(*this, normal, refractionIndex); } \
			inline AlignedType __vectorcall RefractV(const AlignedType& normal, const AlignedType& refractionIndex) const \
				{ return XMVector2RefractV(*this, normal, refractionIndex); } \
			inline AlignedType __vectorcall Orthogonal() const \
				{ return XMVector2Orthogonal(*this); } \
			inline float __vectorcall AngleTo(const AlignedType& rhs) const \
				{ return XMVectorGetX(XMVector2AngleBetweenVectors(*this, rhs)); } \
			inline AlignedType __vectorcall SplatAngleTo(const AlignedType& rhs) const \
				{ return XMVector2AngleBetweenVectors(*this, rhs); } \
			inline float __vectorcall NormalAngleTo(const AlignedType& rhs) const \
				{ return XMVectorGetX(XMVector2AngleBetweenNormals(*this, rhs)); } \
			inline AlignedType __vectorcall SplatNormalAngleTo(const AlignedType& rhs) const \
				{ return XMVector2AngleBetweenNormals(*this, rhs); } \
			inline float __vectorcall DistanceToLine(const AlignedType& linePt1, const AlignedType& linePt2) const \
				{ return XMVectorGetX(XMVector2LinePointDistance(linePt2, linePt2, *this)); } \
			inline AlignedType __vectorcall SplatDistanceToLine(const AlignedType& linePt1, const AlignedType& linePt2) const \
				{ return XMVector2LinePointDistance(linePt2, linePt2, *this); } \
		inline AlignedType __vectorcall Transform(const Float4x3A& m) const; \
		inline AlignedType __vectorcall Transform(const Float4x4A& m) const; \
		inline AlignedType __vectorcall TransformNormal(const Float4x3A& m) const; \
		inline AlignedType __vectorcall TransformNormal(const Float4x4A& m) const; \
		inline AlignedType __vectorcall Project(const Float4x3A& m) const; \
		inline AlignedType __vectorcall Project(const Float4x4A& m) const; \
		inline AlignedType __vectorcall operator *(const Float4x3A& m) const; \
		inline AlignedType __vectorcall operator *(const Float4x4A& m) const; \
		inline Vector& __vectorcall operator *=(const Float4x3A& m); \
		inline Vector& __vectorcall operator *=(const Float4x4A& m);

#define VectorInt2Members \
			inline bool __vectorcall operator ==(const AlignedType& rhs) const { return XMVector2EqualInt(*this, rhs); } \
			inline bool __vectorcall operator !=(const AlignedType& rhs) const { return XMVector2NotEqualInt(*this, rhs); }

#define VectorBool2Members \
			inline bool __vectorcall operator ==(const AlignedType& rhs) const { return XMVector2EqualInt(*this, rhs); } \
			inline bool __vectorcall operator !=(const AlignedType& rhs) const { return XMVector2NotEqualInt(*this, rhs); }

#define VectorFloat3Members \
			inline bool __vectorcall operator ==(const AlignedType& rhs) const { return XMVector3Equal(*this, rhs); } \
			inline bool __vectorcall operator !=(const AlignedType& rhs) const { return XMVector3NotEqual(*this, rhs); } \
			inline bool __vectorcall operator >(const AlignedType& rhs) const { return XMVector3Greater(*this, rhs); } \
			inline bool __vectorcall operator >=(const AlignedType& rhs) const { return XMVector3GreaterOrEqual(*this, rhs); } \
			inline bool __vectorcall operator <(const AlignedType& rhs) const { return XMVector3Less(*this, rhs); } \
			inline bool __vectorcall operator <=(const AlignedType& rhs) const { return XMVector3LessOrEqual(*this, rhs); } \
			inline bool __vectorcall NearEqual(const AlignedType& rhs, const AlignedType& epsilon) const { return XMVector3NearEqual(*this, rhs, epsilon); } \
			inline bool __vectorcall InBounds(const AlignedType& bounds) const { return XMVector3InBounds(*this, bounds); } \
			PROPERTY_READONLY(bool, IsNan); \
			inline bool __vectorcall GetIsNan() const { return XMVector3IsNaN(*this); } \
			PROPERTY_READONLY(bool, IsInfinite); \
			inline bool __vectorcall GetIsInfinite() const { return XMVector3IsInfinite(*this); } \
			inline float __vectorcall Dot(const AlignedType& rhs) const { return XMVectorGetX(XMVector3Dot(*this, rhs)); } \
			inline AlignedType __vectorcall SplatDot(const AlignedType& rhs) const { return XMVector3Dot(*this, rhs); } \
			inline AlignedType __vectorcall Cross(const AlignedType& rhs) const { return XMVector3Cross(*this, rhs); } \
			PROPERTY_READONLY(float, Length); \
			inline float __vectorcall GetLength() const { return XMVectorGetX(XMVector3Length(*this)); } \
			inline AlignedType __vectorcall SplatLength() const { return XMVector3Length(*this); } \
			PROPERTY_READONLY(float, LengthSquared); \
			inline float __vectorcall GetLengthSquared() const { return XMVectorGetX(XMVector3LengthSq(*this)); } \
			inline AlignedType __vectorcall SplatLengthSquared() const { return XMVector3LengthSq(*this); } \
			PROPERTY_READONLY(float, ReciprocalLength); \
			inline float __vectorcall GetReciprocalLength() const { return XMVectorGetX(XMVector3ReciprocalLength(*this)); } \
			inline AlignedType __vectorcall SplatReciprocalLength() const { return XMVector3ReciprocalLength(*this); } \
			inline AlignedType __vectorcall Normalize() const { return XMVector3Normalize(*this); } \
			inline AlignedType __vectorcall ClampLength(float minLength, float maxLength) const \
				{ return XMVector3ClampLength(*this, minLength, maxLength); } \
			inline AlignedType __vectorcall ClampLengthV(const AlignedType& minLength, const AlignedType& maxLength) const \
				{ return XMVector3ClampLengthV(*this, minLength, maxLength); } \
			inline AlignedType __vectorcall Reflect(const AlignedType& normal) const \
				{ return XMVector3Reflect(*this, normal); } \
			inline AlignedType __vectorcall Refract(const AlignedType& normal, float refractionIndex) const \
				{ return XMVector3Refract(*this, normal, refractionIndex); } \
			inline AlignedType __vectorcall RefractV(const AlignedType& normal, const AlignedType& refractionIndex) const \
				{ return XMVector3RefractV(*this, normal, refractionIndex); } \
			inline AlignedType __vectorcall Orthogonal() const \
				{ return XMVector3Orthogonal(*this); } \
			inline float __vectorcall AngleTo(const AlignedType &rhs) const \
				{ return XMVectorGetX(XMVector3AngleBetweenVectors(*this, rhs)); } \
			inline AlignedType __vectorcall SplatAngleTo(const AlignedType& rhs) const \
				{ return XMVector3AngleBetweenVectors(*this, rhs); } \
			inline float __vectorcall NormalAngleTo(const AlignedType& rhs) const \
				{ return XMVectorGetX(XMVector3AngleBetweenNormals(*this, rhs)); } \
			inline AlignedType __vectorcall SplatNormalAngleTo(const AlignedType& rhs) const \
				{ return XMVector3AngleBetweenNormals(*this, rhs); } \
			inline float __vectorcall DistanceToLine(const AlignedType& linePt1, const AlignedType& linePt2) const \
				{ return XMVectorGetX(XMVector3LinePointDistance(linePt2, linePt2, *this)); } \
			inline AlignedType __vectorcall SplatDistanceToLine(const AlignedType& linePt1, const AlignedType& linePt2) const \
				{ return XMVector3LinePointDistance(linePt2, linePt2, *this); }  \
			inline AlignedType __vectorcall Rotate(const QuaternionA& q) const; \
			inline AlignedType __vectorcall Transform(const Float4x3A& m) const; \
			/*inline AlignedType __vectorcall Transform(const Float4x4A& m) const;*/ \
			inline AlignedType __vectorcall TransformNormal(const Float4x3A& m) const; \
			/*inline AlignedType __vectorcall TransformNormal(const Float4x4A& m) const;*/ \
			inline AlignedType __vectorcall Project(const Float4x4A& m) const; \
			inline AlignedType __vectorcall operator *(const Float4x3A& m) const; \
			/*inline AlignedType __vectorcall operator *(const Float4x4A& m) const;*/ \
			inline Vector& __vectorcall operator *=(const Float4x3A& m); \
			/*inline Vector& __vectorcall operator *=(const Float4x4A& m); */

#define VectorInt3Members \
			inline bool __vectorcall operator ==(const AlignedType& rhs) const { return XMVector3EqualInt(*this, rhs); } \
			inline bool __vectorcall operator !=(const AlignedType& rhs) const { return XMVector3NotEqualInt(*this, rhs); }

#define VectorBool3Members \
			inline bool __vectorcall operator ==(const AlignedType& rhs) const { return XMVector3EqualInt(*this, rhs); } \
			inline bool __vectorcall operator !=(const AlignedType& rhs) const { return XMVector3NotEqualInt(*this, rhs); }

#define VectorFloat4Members \
			inline bool __vectorcall operator ==(const AlignedType& rhs) const { return XMVector4Equal(*this, rhs); } \
			inline bool __vectorcall operator !=(const AlignedType& rhs) const { return XMVector4NotEqual(*this, rhs); } \
			inline bool __vectorcall operator >(const AlignedType& rhs) const { return XMVector4Greater(*this, rhs); } \
			inline bool __vectorcall operator >=(const AlignedType& rhs) const { return XMVector4GreaterOrEqual(*this, rhs); } \
			inline bool __vectorcall operator <(const AlignedType& rhs) const { return XMVector4Less(*this, rhs); } \
			inline bool __vectorcall operator <=(const AlignedType& rhs) const { return XMVector4LessOrEqual(*this, rhs); } \
			inline bool __vectorcall NearEqual(const AlignedType& rhs, const AlignedType& epsilon) const { return XMVector4NearEqual(*this, rhs, epsilon); } \
			inline bool __vectorcall InBounds(const AlignedType& bounds) const { return XMVector4InBounds(*this, bounds); } \
			PROPERTY_READONLY(bool, IsNan); \
			inline bool __vectorcall GetIsNan() const { return XMVector4IsNaN(*this); } \
			PROPERTY_READONLY(bool, IsInfinite); \
			inline bool __vectorcall GetIsInfinite() const { return XMVector4IsInfinite(*this); } \
			inline float __vectorcall Dot(const AlignedType& rhs) const { return XMVectorGetX(XMVector4Dot(*this, rhs)); } \
			inline AlignedType __vectorcall SplatDot(const AlignedType& rhs) const { return XMVector4Dot(*this, rhs); } \
			inline AlignedType __vectorcall Cross(const AlignedType& v1, const AlignedType& v2) const { return XMVector4Cross(*this, v1, v2); } \
			PROPERTY_READONLY(float, Length); \
			inline float __vectorcall GetLength() const { return XMVectorGetX(XMVector4Length(*this)); } \
			inline AlignedType __vectorcall SplatLength() const { return XMVector4Length(*this); } \
			PROPERTY_READONLY(float, LengthSquared); \
			inline float __vectorcall GetLengthSquared() const { return XMVectorGetX(XMVector4LengthSq(*this)); } \
			inline AlignedType __vectorcall SplatLengthSquared() const { return XMVector4LengthSq(*this); } \
			PROPERTY_READONLY(float, ReciprocalLength); \
			inline float __vectorcall GetReciprocalLength() const { return XMVectorGetX(XMVector4ReciprocalLength(*this)); } \
			inline AlignedType __vectorcall SplatReciprocalLength() const { return XMVector4ReciprocalLength(*this); } \
			inline AlignedType __vectorcall Normalize() const { return XMVector4Normalize(*this); } \
			inline AlignedType __vectorcall ClampLength(float minLength, float maxLength) const \
				{ return XMVector4ClampLength(*this, minLength, maxLength); } \
			inline AlignedType __vectorcall ClampLengthV(const AlignedType& minLength, const AlignedType& maxLength) const \
				{ return XMVector3ClampLengthV(*this, minLength, maxLength); } \
			inline AlignedType __vectorcall Orthogonal() const \
				{ return XMVector4Orthogonal(*this); } \
			/*inline AlignedType __vectorcall Transform(const Float4x3A& m) const;*/ \
			inline AlignedType __vectorcall Transform(const Float4x4A& m) const; \
			/*inline AlignedType __vectorcall operator *(const Float4x3A& m) const;*/ \
			inline AlignedType __vectorcall operator *(const Float4x4A& m) const; \
			/*inline Vector& __vectorcall operator *=(const Float4x3A& m);*/ \
			inline Vector& __vectorcall operator *=(const Float4x4A& m);

#define VectorInt4Members \
			inline bool __vectorcall operator ==(const AlignedType& rhs) const { return XMVector4EqualInt(*this, rhs); } \
			inline bool __vectorcall operator !=(const AlignedType& rhs) const { return XMVector4NotEqualInt(*this, rhs); }

#define VectorBool4Members \
			inline bool __vectorcall operator ==(const AlignedType& rhs) const { return XMVector4EqualInt(*this, rhs); } \
			inline bool __vectorcall operator !=(const AlignedType& rhs) const { return XMVector4NotEqualInt(*this, rhs); }

#define VectorFloatMembers(Rank,Alignment) \
		Vector##Rank##Data##Alignment(float) \
		VectorSwizzle##Alignment(float,Rank) \
		VectorFloat##Rank##Members \
		static inline AlignedType __vectorcall Min(const AlignedType& v1, const AlignedType& v2) { return XMVectorMin(v1, v2); }; \
		static inline AlignedType __vectorcall Max(const AlignedType& v1, const AlignedType& v2) { return XMVectorMax(v1, v2); }; \
		static inline AlignedType __vectorcall Lerp(const AlignedType& v1, const AlignedType& v2, float u) { return XMVectorLerp(v1, v2, u); } \
		static inline AlignedType __vectorcall LerpV(const AlignedType& v1, const AlignedType& v2, const AlignedType& u) { return XMVectorLerpV(v1, v2, u); } \
		static inline AlignedType __vectorcall Hermite( \
			const AlignedType& position1, \
			const AlignedType& tangent1, \
			const AlignedType& position2, \
			const AlignedType& tangent2, \
			float u) \
			{ return XMVectorHermite(position1, tangent1, position2, tangent2, u); } \
		static inline AlignedType __vectorcall HermiteV( \
			const AlignedType& position1, \
			const AlignedType& tangent1, \
			const AlignedType& position2, \
			const AlignedType& tangent2, \
			const AlignedType& u) \
			{ return XMVectorHermiteV(position1, tangent1, position2, tangent2, u); } \
		static inline AlignedType __vectorcall CatmullRom( \
			const AlignedType& v1, \
			const AlignedType& v2, \
			const AlignedType& v3, \
			const AlignedType& v4, \
			float u) \
			{ return XMVectorCatmullRom(v1, v2, v3, v4, u); } \
		static inline AlignedType __vectorcall CatmullRomV( \
			const AlignedType& v1, \
			const AlignedType& v2, \
			const AlignedType& v3, \
			const AlignedType& v4, \
			const AlignedType& u) \
			{ return XMVectorCatmullRomV(v1, v2, v3, v4, u); } \
		static inline AlignedType __vectorcall Barycentric( \
			const AlignedType& v1, \
			const AlignedType& v2, \
			const AlignedType& v3, \
			float f, \
			float g) \
			{ return XMVectorBaryCentric(v1, v2, v3, f, g); } \
		static inline AlignedType __vectorcall BarycentricV( \
			const AlignedType& v1, \
			const AlignedType& v2, \
			const AlignedType& v3, \
			const AlignedType& f, \
			const AlignedType& g) \
			{ return XMVectorBaryCentricV(v1, v2, v3, f, g); } \
		inline AlignedType __vectorcall Round() const { return XMVectorRound(*this); } \
		inline AlignedType __vectorcall Truncate() const { return XMVectorTruncate(*this); } \
		inline AlignedType __vectorcall Floor() const { return XMVectorFloor(*this); } \
		inline AlignedType __vectorcall Ceiling() const { return XMVectorCeiling(*this); } \
		inline AlignedType __vectorcall Clamp(const AlignedType& low, const AlignedType& high) const { return XMVectorClamp(*this, low, high); } \
		inline AlignedType __vectorcall Saturate() const { return XMVectorSaturate(*this); } \
		inline AlignedType __vectorcall AddAngles(const AlignedType& rhs) const { return XMVectorAddAngles(*this, rhs); } \
		inline AlignedType __vectorcall SubtractAngles(const AlignedType& rhs) const { return XMVectorSubtractAngles(*this, rhs); } \
		inline AlignedType __vectorcall ModAngles() const { return XMVectorModAngles(*this); } \
		inline AlignedType __vectorcall MultiplyThenAdd(const AlignedType& mul1, const AlignedType& mul2) const { return XMVectorMultiplyAdd(mul1, mul2, *this); } \
		inline AlignedType __vectorcall MultiplyThenSubtract(const AlignedType& mul1, const AlignedType& mul2) const { return XMVectorNegativeMultiplySubtract(mul1, mul2, *this); } \
		inline AlignedType __vectorcall Reciprocal() const { return XMVectorReciprocal(*this); } \
		inline AlignedType __vectorcall Sqrt() const { return XMVectorSqrt(*this); } \
		inline AlignedType __vectorcall ReciprocalSqrt() const { return XMVectorReciprocalSqrt(*this); } \
		inline AlignedType __vectorcall Exp() const { return XMVectorExpE(*this); } \
		inline AlignedType __vectorcall Exp2() const { return XMVectorExp2(*this); } \
		inline AlignedType __vectorcall Ln() const { return XMVectorLogE(*this); } \
		inline AlignedType __vectorcall Log2() const { return XMVectorLog2(*this); } \
		inline AlignedType __vectorcall Pow(const AlignedType& rhs) const { return XMVectorPow(*this, rhs); } \
		inline AlignedType __vectorcall Abs() const { return XMVectorAbs(*this); } \
		inline AlignedType __vectorcall Sin() const { return XMVectorSin(*this); } \
		inline AlignedType __vectorcall Cos() const { return XMVectorCos(*this); } \
		inline AlignedType __vectorcall Tan() const { return XMVectorTan(*this); } \
		inline AlignedType __vectorcall SinH() const { return XMVectorSinH(*this); } \
		inline AlignedType __vectorcall CosH() const { return XMVectorCosH(*this); } \
		inline AlignedType __vectorcall TanH() const { return XMVectorTanH(*this); } \
		inline AlignedType __vectorcall ASin() const { return XMVectorASin(*this); } \
		inline AlignedType __vectorcall ACos() const { return XMVectorACos(*this); } \
		inline AlignedType __vectorcall ATan() const { return XMVectorATan(*this); } \
		static inline AlignedType __vectorcall ATan2(const AlignedType& y, const AlignedType& x) { return XMVectorATan2(y, x); } \
		inline void __vectorcall SinCos(const AlignedType& outSin, const AlignedType& outCos) const { return XMVectorSinCos((XMVECTOR*)&outSin, (XMVECTOR*)&outCos, *this); } \
		inline AlignedType __vectorcall operator-() const { return XMVectorNegate(*this); } \
		inline AlignedType __vectorcall operator+() const { return AlignedType(*this); } \
		inline AlignedType __vectorcall operator+(const AlignedType& rhs) const { return XMVectorAdd(*this, rhs); } \
		inline AlignedType __vectorcall operator-(const AlignedType& rhs) const { return XMVectorSubtract(*this, rhs); } \
		inline AlignedType __vectorcall operator*(const AlignedType& rhs) const { return XMVectorMultiply(*this, rhs); } \
		inline AlignedType __vectorcall operator/(const AlignedType& rhs) const { return XMVectorDivide(*this, rhs); } \
		inline AlignedType __vectorcall operator*(float rhs) const { return XMVectorScale(*this, rhs); } \
		inline AlignedType __vectorcall operator/(float rhs) const { return XMVectorScale(*this, 1.0f / rhs); } \
		inline AlignedType __vectorcall operator%(const AlignedType& rhs) const { return XMVectorMod(*this, rhs); }  \
		inline Vector& __vectorcall operator-=(const AlignedType& rhs) { (*this) = (*this) - rhs; return *this; } \
		inline Vector& __vectorcall operator+=(const AlignedType& rhs) { (*this) = (*this) + rhs; return *this; } \
		inline Vector& __vectorcall operator*=(const AlignedType& rhs) { (*this) = (*this) * rhs; return *this; } \
		inline Vector& __vectorcall operator/=(const AlignedType& rhs) { (*this) = (*this) / rhs; return *this; } \
		inline Vector& __vectorcall operator*=(float rhs) { (*this) = (*this) * rhs; return *this; } \
		inline Vector& __vectorcall operator/=(float rhs) { (*this) = (*this) / rhs; return *this; } \
		struct _Estimate { const Vector& _v; _Estimate(const Vector& v) : _v(v) { } \
			inline AlignedType __vectorcall Reciprocal() const { return XMVectorReciprocalEst(_v); } \
			inline AlignedType __vectorcall Sqrt() const { return XMVectorSqrtEst(_v); } \
			inline AlignedType __vectorcall ReciprocalSqrt() const { return XMVectorReciprocalSqrtEst(_v); } \
			inline AlignedType __vectorcall Sin() const { return XMVectorSinEst(_v); } \
			inline AlignedType __vectorcall Cos() const { return XMVectorCosEst(_v); } \
			inline AlignedType __vectorcall Tan() const { return XMVectorTanEst(_v); } \
			inline AlignedType __vectorcall ASin() const { return XMVectorASinEst(_v); } \
			inline AlignedType __vectorcall ACos() const { return XMVectorACosEst(_v); } \
			inline AlignedType __vectorcall ATan() const { return XMVectorATanEst(_v); } \
			static inline AlignedType __vectorcall ATan2(const AlignedType& y, const AlignedType& x) { return XMVectorATan2Est(y, x); } \
			inline float __vectorcall Length() const { return XMVectorGetX(XMVector##Rank##LengthEst(_v)); } \
			inline AlignedType __vectorcall SplatLength() const { return XMVector##Rank##LengthEst(_v); } \
			inline float __vectorcall ReciprocalLength() const { return XMVectorGetX(XMVector##Rank##ReciprocalLengthEst(_v)); } \
			inline AlignedType __vectorcall SplatReciprocalLength() const { return XMVector##Rank##ReciprocalLengthEst(_v); } \
			inline AlignedType __vectorcall Normalize() const { return XMVector##Rank##NormalizeEst(_v); } }; \
		_Estimate GetEstimate() const { return _Estimate(*this); } \
		PROPERTY_READONLY(_Estimate, Estimate); \
		struct _ComponentWise { const Vector& _v; _ComponentWise(const Vector& v) : _v(v) { } \
			typedef Vector<bool, Rank, true> OpType; \
			inline OpType __vectorcall operator ==(const AlignedType& rhs) const { return XMVectorEqual(_v, rhs); } \
			inline OpType __vectorcall operator !=(const AlignedType& rhs) const { return XMVectorNotEqual(_v, rhs); } \
			inline OpType __vectorcall operator <(const AlignedType& rhs) const { return XMVectorLess(_v, rhs); } \
			inline OpType __vectorcall operator <=(const AlignedType& rhs) const { return XMVectorLessOrEqual(_v, rhs); } \
			inline OpType __vectorcall operator >(const AlignedType& rhs) const { return XMVectorGreater(_v, rhs); } \
			inline OpType __vectorcall operator >=(const AlignedType& rhs) const { return XMVectorGreaterOrEqual(_v, rhs); } \
			inline OpType __vectorcall NearEqual(const AlignedType& rhs, const AlignedType& epsilon) const { return XMVectorNearEqual(_v, rhs, epsilon); } \
			inline OpType __vectorcall IsInBounds(const AlignedType& bounds) const { return XMVectorInBounds(_v, bounds); } \
			inline OpType __vectorcall IsNan() const { return XMVectorIsNaN(_v); } \
			inline OpType __vectorcall IsInfinite() const { return XMVectorIsInfinite(_v); } }; \
		_ComponentWise GetComponentWise() const { return _ComponentWise(*this); } \
		PROPERTY_READONLY(_ComponentWise, ComponentWise);

	template<int Rank, bool Align>
	inline Vector<float, Rank, true> __vectorcall operator *(float lhs, const Vector<float, Rank, Align>& rhs)
	{
		return XMVectorScale(rhs, lhs);
	}

	template<int Rank, bool Align>
	inline Vector<float, Rank, true> __vectorcall operator /(float lhs, const Vector<float, Rank, Align>& rhs)
	{
		return XMVectorScale(rhs.Reciprocal(), lhs);
	}

#define VectorIntMembers(Rank,Alignment) \
		Vector##Rank##Data##Alignment(int) \
		VectorSwizzle##Alignment(int,Rank) \
		VectorInt##Rank##Members \
		inline AlignedType __vectorcall BitwiseAnd(const AlignedType& rhs) const { return XMVectorAndInt(*this, rhs); } \
		inline AlignedType __vectorcall BitwiseOr(const AlignedType& rhs) const { return XMVectorOrInt(*this, rhs); } \
		inline AlignedType __vectorcall BitwiseExclusiveOr(const AlignedType& rhs) const { return XMVectorXorInt(*this, rhs); } \
		inline AlignedType __vectorcall BitwiseAndNot(const AlignedType& rhs) const { return XMVectorAndCInt(*this, rhs); } \
		inline AlignedType __vectorcall BitwiseNor(const AlignedType& rhs) const { return XMVectorNorInt(*this, rhs); } \
		inline AlignedType __vectorcall operator &(const AlignedType& rhs) const { return XMVectorAndInt(*this, rhs); } \
		inline AlignedType __vectorcall operator |(const AlignedType& rhs) const { return XMVectorOrInt(*this, rhs); } \
		inline AlignedType __vectorcall operator ^(const AlignedType& rhs) const { return XMVectorXorInt(*this, rhs); } \
		struct _ComponentWise { const Vector& _v; _ComponentWise(const Vector& v) : _v(v) { } \
			typedef Vector<bool, Rank, true> OpType; \
			inline OpType __vectorcall operator ==(const AlignedType& rhs) const { return XMVectorEqualInt(_v, rhs); } \
			inline OpType __vectorcall operator !=(const AlignedType& rhs) const { return XMVectorNotEqualInt(_v, rhs); } }; \
		_ComponentWise GetComponentWise() const { return _ComponentWise(*this); } \
		PROPERTY_READONLY(_ComponentWise, ComponentWise);

#define VectorBoolMembers(Rank, Alignment) \
		Vector##Rank##Data##Alignment(bool) \
		VectorSwizzle##Alignment(bool,Rank) \
		VectorBool##Rank##Members

#define VectorAlignedFloatConstructors(Rank) \
	Vector() { } \
	Vector(XMVECTOR xm) : _xm(xm) { } \
	Vector(const UnalignedType& ua); \
	Vector(float repeat) : _xm(XMVectorReplicate(repeat)) { } \
	explicit Vector(const float* arr) : _xm(XMLoadFloat##Rank((XMFLOAT##Rank*)arr)) { }

#define VectorAlignedIntConstructors(Rank) \
	Vector() { } \
	Vector(XMVECTOR xm) : _xm(xm) { } \
	Vector(const UnalignedType& ua); \
	Vector(int repeat) : _xm(XMVectorReplicateInt(repeat)) { } \
	explicit Vector(const int* arr) : _xm(XMLoadInt##Rank((uint32_t*)arr)) { }

#define VectorAlignedBoolArrayConstructor_2 explicit Vector(const bool* arr) : _xm(XMVectorSetInt(arr[0] ? 0xFFFFFFFF : 0, arr[1] ? 0xFFFFFFFF : 1, 0, 0)) { }
#define VectorAlignedBoolArrayConstructor_3 explicit Vector(const bool* arr) : _xm(XMVectorSetInt(arr[0] ? 0xFFFFFFFF : 0, arr[1] ? 0xFFFFFFFF : 1, arr[2] ? 0xFFFFFFFF : 0, 0)) { }
#define VectorAlignedBoolArrayConstructor_4 explicit Vector(const bool* arr) : _xm(XMVectorSetInt(arr[0] ? 0xFFFFFFFF : 0, arr[1] ? 0xFFFFFFFF : 1, arr[2] ? 0xFFFFFFFF : 0, arr[3] ? 0xFFFFFFFF : 0)) { }

#define VectorAlignedBoolConstructors(Rank) \
	Vector() { } \
	Vector(const UnalignedType& ua); \
	Vector(XMVECTOR xm) : _xm(xm) { } \
	Vector(bool repeat) : _xm(XMVectorReplicateInt(repeat ? 0xFFFFFFFF : 0)) { } \
	VectorAlignedBoolArrayConstructor_##Rank

	template<>
	struct Vector<bool, 2, true> : public VectorBaseTraits<bool, 2>
	{
		VectorAlignedBoolConstructors(2)
			Vector(bool x, bool y) : _xm(XMVectorSetInt(x ? 0xFFFFFFFF : 0, y ? 0xFFFFFFFF : 0, 0, 0)) {}
		inline Vector& __vectorcall operator=(const Vector& rhs) { _xm = rhs; return *this; }
		//inline Vector& __vectorcall operator=(const UnalignedType& rhs) { _xm = XMVectorSetInt(rhs.X ? 0xFFFFFFFF : 0, rhs.Y ? 0xFFFFFFFF : 0, 0, 0); return *this; }

		inline bool __vectorcall Any() const { return XMVector2NotEqual(_xm, False); }
		inline bool __vectorcall All() const { return XMVector2Equal(_xm, True); }

		VectorBoolMembers(2, Aligned)

			static const Vector True;
		static const Vector False;
	};

	template<>
	struct Vector<bool, 2, false> : public VectorBaseTraits<bool, 2>
	{
		Vector() { }
		Vector(XMVECTOR xm) : X(XMVectorGetIntX(xm) != 0), Y(XMVectorGetIntY(xm) != 0) { }
		Vector(const AlignedType& a) : X(a.X), Y(a.Y) { }
		Vector(bool repeat) : X(repeat), Y(repeat) { }
		explicit Vector(const bool* arr) : X(arr[0]), Y(arr[1]) { }
		Vector(bool x, bool y) : X(x), Y(y) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; return *this; }
		inline Vector& __vectorcall operator=(const AlignedType& rhs) { X = rhs.X; Y = rhs.Y; return *this; }

		inline bool Any() const { return X || Y; }
		inline bool All() const { return X && Y; }

		VectorBoolMembers(2, Unaligned)

			inline operator XMVECTOR() const { return XMVectorSetInt(X ? 0xFFFFFFFF : 0, Y ? 0xFFFFFFFF : 0, 0, 0); }

		static const Vector True;
		static const Vector False;
	};

	template<>
	struct Vector<bool, 3, true> : public VectorBaseTraits<bool, 3>
	{
		VectorAlignedBoolConstructors(3)
			Vector(bool x, bool y, bool z) : _xm(XMVectorSetInt(x ? 0xFFFFFFFF : 0, y ? 0xFFFFFFFF : 0, z ? 0xFFFFFFFF : 0, 0)) {}
		inline Vector& __vectorcall operator=(const Vector& rhs) { _xm = rhs; return *this; }
		//inline Vector& __vectorcall operator=(const UnalignedType& rhs) { _xm = XMVectorSetInt(rhs.X ? 0xFFFFFFFF : 0, rhs.Y ? 0xFFFFFFFF : 0, rhs.Z ? 0xFFFFFFFF : 0, 0); return *this; }

		inline bool __vectorcall Any() const { return XMVector3NotEqual(_xm, False); }
		inline bool __vectorcall All() const { return XMVector3Equal(_xm, True); }

		VectorBoolMembers(3, Aligned)

			static const Vector True;
		static const Vector False;
	};

	template<>
	struct Vector<bool, 3, false> : public VectorBaseTraits<bool, 3>
	{
		Vector() { }
		Vector(XMVECTOR xm) : X(XMVectorGetIntX(xm) != 0), Y(XMVectorGetIntY(xm) != 0), Z(XMVectorGetIntZ(xm) != 0) { }
		Vector(const AlignedType& a) : X(a.X), Y(a.Y), Z(a.Z) { }
		Vector(bool repeat) : X(repeat), Y(repeat), Z(repeat) { }
		explicit Vector(const bool* arr) : X(arr[0]), Y(arr[1]), Z(arr[2]) { }
		Vector(bool x, bool y, bool z) : X(x), Y(y), Z(z) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; Z = rhs.Z; return *this; }
		inline Vector& __vectorcall operator=(const AlignedType& rhs) { X = rhs.X; Y = rhs.Y; Z = rhs.Z; return *this; }

		inline bool Any() const { return X || Y || Z; }
		inline bool All() const { return X && Y && Z; }

		VectorBoolMembers(3, Unaligned)

			inline operator XMVECTOR() const { return XMVectorSetInt(X ? 0xFFFFFFFF : 0, Y ? 0xFFFFFFFF : 0, Z ? 0xFFFFFFFF : 0, 0); }

		static const Vector True;
		static const Vector False;
	};

	template<>
	struct Vector<bool, 4, true> : public VectorBaseTraits<bool, 4>
	{
		VectorAlignedBoolConstructors(4)
			Vector(bool x, bool y, bool z, bool w) : _xm(XMVectorSetInt(x ? 0xFFFFFFFF : 0, y ? 0xFFFFFFFF : 0, z ? 0xFFFFFFFF : 0, w ? 0xFFFFFFFF : 0)) {}
		inline Vector& __vectorcall operator=(const Vector& rhs) { _xm = rhs; return *this; }
		//inline Vector& __vectorcall operator=(const UnalignedType& rhs) { _xm = XMVectorSetInt(rhs.X ? 0xFFFFFFFF : 0, rhs.Y ? 0xFFFFFFFF : 0, rhs.Z ? 0xFFFFFFFF : 0, rhs.W ? 0xFFFFFFFF : 0); return *this; }

		inline bool __vectorcall Any() const { return XMVector4NotEqual(_xm, False); }
		inline bool __vectorcall All() const { return XMVector4Equal(_xm, True); }

		VectorBoolMembers(4, Aligned)

			static const Vector True;
		static const Vector False;
	};

	template<>
	struct Vector<bool, 4, false> : public VectorBaseTraits<bool, 4>
	{
		Vector() { }
		Vector(XMVECTOR xm) : X(XMVectorGetIntX(xm) != 0), Y(XMVectorGetIntY(xm) != 0), Z(XMVectorGetIntZ(xm) != 0), W(XMVectorGetIntW(xm) != 0) { }
		Vector(const AlignedType& a) : X(a.X), Y(a.Y), Z(a.Z), W(a.W) { }
		Vector(bool repeat) : X(repeat), Y(repeat), Z(repeat), W(repeat) { }
		explicit Vector(const bool* arr) : X(arr[0]), Y(arr[1]), Z(arr[2]), W(arr[3]) { }
		Vector(bool x, bool y, bool z, bool w) : X(x), Y(y), Z(z), W(w) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; Z = rhs.Z; W = rhs.W; return *this; }
		inline Vector& __vectorcall operator=(const AlignedType& rhs) { X = rhs.X; Y = rhs.Y; Z = rhs.Z; W = rhs.W; return *this; }

		inline bool Any() const { return X || Y || Z || W; }
		inline bool All() const { return X && Y && Z && W; }

		VectorBoolMembers(4, Unaligned)

			inline operator XMVECTOR() const { return XMVectorSetInt(X ? 0xFFFFFFFF : 0, Y ? 0xFFFFFFFF : 0, Z ? 0xFFFFFFFF : 0, W ? 0xFFFFFFFF : 0); }

		static const Vector True;
		static const Vector False;
	};

	template<>
	struct Vector<float, 2, true> : public VectorBaseTraits<float, 2>
	{
		VectorAlignedFloatConstructors(2)
			Vector(float x, float y) : _xm(XMVectorSet(x, y, 0, 1)) {}
		inline Vector& __vectorcall operator=(const Vector& rhs) { _xm = rhs; return *this; }
		//inline Vector& __vectorcall operator=(const UnalignedType& rhs) { _xm = rhs; return *this; }

		VectorFloatMembers(2, Aligned)

			static const Vector Zero;
		static const Vector One;
		static const Vector Infinity;
		static const Vector QNaN;
		static const Vector Epsilon;
		static const Vector SignMask;
	};

	template<>
	struct Vector<float, 2, false> : public VectorBaseTraits<float, 2>
	{
		Vector() { }
		Vector(XMVECTOR xm) { XMStoreFloat2((XMFLOAT2*)this, xm); }
		Vector(const AlignedType& a) { XMStoreFloat2((XMFLOAT2*)this, a); }
		Vector(float repeat) : X(repeat), Y(repeat) { }
		explicit Vector(const float* arr) : X(arr[0]), Y(arr[1]) { }
		Vector(float x, float y) : X(x), Y(y) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; return *this; }
		inline Vector& __vectorcall operator=(const AlignedType& rhs) { XMStoreFloat2((XMFLOAT2*)this, rhs); return *this; }

		VectorFloatMembers(2, Unaligned)

			inline operator XMVECTOR() const { return XMLoadFloat2((XMFLOAT2*)this); }

		static const Vector Zero;
		static const Vector One;
		static const Vector Infinity;
		static const Vector QNaN;
		static const Vector Epsilon;
		static const Vector SignMask;
	};

	template<>
	struct Vector<float, 3, true> : public VectorBaseTraits<float, 3>
	{
		VectorAlignedFloatConstructors(3)

			Vector(const Vector<float, 2, true>& xy, float z) : _xm(XMVectorSet(xy.X, xy.Y, z, 1)) { }
		Vector(const Vector<float, 2, false>& xy, float z) : _xm(XMVectorSet(xy.X, xy.Y, z, 1)) { }
		Vector(float x, float y, float z) : _xm(XMVectorSet(x, y, z, 1)) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { _xm = rhs; return *this; }
		//inline Vector& __vectorcall operator=(const UnalignedType& rhs) { _xm = rhs; return *this; }

		VectorFloatMembers(3, Aligned)

			static const Vector Zero;
		static const Vector One;
		static const Vector Infinity;
		static const Vector QNaN;
		static const Vector Epsilon;
		static const Vector SignMask;
	};

	template<>
	struct Vector<float, 3, false> : public VectorBaseTraits<float, 3>
	{
		Vector() { }
		Vector(XMVECTOR xm) { XMStoreFloat3((XMFLOAT3*)this, xm); }
		Vector(const AlignedType& a) { XMStoreFloat3((XMFLOAT3*)this, a); }
		Vector(float repeat) : X(repeat), Y(repeat), Z(repeat) { }
		explicit Vector(const float* arr) : X(arr[0]), Y(arr[1]), Z(arr[2]) { }
		Vector(const Vector<float, 2, false>& xy, float z) : X(xy.X), Y(xy.Y), Z(z) { }
		Vector(const Vector<float, 2, true>& xy, float z) : X(xy.X), Y(xy.Y), Z(z) { }
		Vector(float x, float y, float z) : X(x), Y(y), Z(z) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; Z = rhs.Z;  return *this; }
		inline Vector& __vectorcall operator=(const AlignedType& rhs) { XMStoreFloat3((XMFLOAT3*)this, rhs); return *this; }

		VectorFloatMembers(3, Unaligned)

			inline operator XMVECTOR() const { return XMLoadFloat3((XMFLOAT3*)this); }

		static const Vector Zero;
		static const Vector One;
		static const Vector Infinity;
		static const Vector QNaN;
		static const Vector Epsilon;
		static const Vector SignMask;
	};

	template<>
	struct Vector<float, 4, true> : public VectorBaseTraits<float, 4>
	{
		VectorAlignedFloatConstructors(4)

			Vector(const Vector<float, 3, false>& xyz, float w) : _xm(XMVectorSet(xyz.X, xyz.Y, xyz.Z, w)) { }
		Vector(const Vector<float, 3, true>& xyz, float w) : _xm(XMVectorSetW(xyz, w)) { }
		Vector(float x, float y, float z, float w) : _xm(XMVectorSet(x, y, z, w)) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { _xm = rhs; return *this; }
		//inline Vector& __vectorcall operator=(const UnalignedType& rhs) { _xm = rhs; return *this; }

		VectorFloatMembers(4, Aligned)

			static const Vector Zero;
		static const Vector One;
		static const Vector Infinity;
		static const Vector QNaN;
		static const Vector Epsilon;
		static const Vector SignMask;
	};

	template<>
	struct Vector<float, 4, false> : public VectorBaseTraits<float, 4>
	{
		Vector() { }
		Vector(XMVECTOR xm) { XMStoreFloat4((XMFLOAT4*)this, xm); }
		Vector(const AlignedType& a) { XMStoreFloat4((XMFLOAT4*)this, a); }
		Vector(float repeat) : X(repeat), Y(repeat), Z(repeat), W(repeat) { }
		explicit Vector(const float* arr) : X(arr[0]), Y(arr[1]), Z(arr[2]), W(arr[3]) { }
		Vector(const Vector<float, 3, false>& xyz, float w) : X(xyz.X), Y(xyz.Y), Z(xyz.Z), W(w) { }
		Vector(const Vector<float, 3, true>& xyz, float w) : X(xyz.X), Y(xyz.Y), Z(xyz.Z), W(w) { }
		Vector(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; Z = rhs.Z; W = rhs.W; return *this; }
		inline Vector& __vectorcall operator=(const AlignedType& rhs) { XMStoreFloat4((XMFLOAT4*)this, rhs); return *this; }

		VectorFloatMembers(4, Unaligned)

			inline operator XMVECTOR() const { return XMLoadFloat4((XMFLOAT4*)this); }

		static const Vector Zero;
		static const Vector One;
		static const Vector Infinity;
		static const Vector QNaN;
		static const Vector Epsilon;
		static const Vector SignMask;
	};

	template<>
	struct Vector<int, 2, true> : public VectorBaseTraits<int, 2>
	{
		VectorAlignedIntConstructors(2)
			Vector(int x, int y) : _xm(XMVectorSetInt(x, y, 0, 1)) {}
		inline Vector& __vectorcall operator=(const Vector& rhs) { _xm = rhs; return *this; }
		//inline Vector& __vectorcall operator=(const UnalignedType& rhs) { _xm = rhs; return *this; }

		VectorIntMembers(2, Aligned)

			static const Vector Zero;
		static const Vector One;
		static const Vector NegativeOne;
	};

	template<>
	struct Vector<int, 2, false> : public VectorBaseTraits<int, 2>
	{
		Vector() { }
		Vector(XMVECTOR xm) { XMStoreInt2((uint32_t*)this, xm); }
		Vector(const AlignedType& a) { XMStoreInt2((uint32_t*)this, a); }
		Vector(int repeat) : X(repeat), Y(repeat) { }
		explicit Vector(const int* arr) : X(arr[0]), Y(arr[1]) { }
		Vector(int x, int y) : X(x), Y(y) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; return *this; }
		inline Vector& __vectorcall operator=(const AlignedType& rhs) { XMStoreInt2((uint32_t*)this, rhs); return *this; }

		VectorIntMembers(2, Unaligned)

			inline operator XMVECTOR() const { return XMLoadInt2((uint32_t*)this); }

		static const Vector Zero;
		static const Vector One;
		static const Vector NegativeOne;
	};

	template<>
	struct Vector<int, 3, true> : public VectorBaseTraits<int, 3>
	{
		VectorAlignedIntConstructors(3)
			Vector(int x, int y, int z) : _xm(XMVectorSetInt(x, y, z, 1)) {}
		inline Vector& __vectorcall operator=(const Vector& rhs) { _xm = rhs; return *this; }
		//inline Vector& __vectorcall operator=(const UnalignedType& rhs) { _xm = rhs; return *this; }

		VectorIntMembers(3, Aligned)

			static const Vector Zero;
		static const Vector One;
		static const Vector NegativeOne;
	};

	template<>
	struct Vector<int, 3, false> : public VectorBaseTraits<int, 3>
	{
		Vector() { }
		Vector(XMVECTOR xm) { XMStoreInt3((uint32_t*)this, xm); }
		Vector(const AlignedType& a) { XMStoreInt3((uint32_t*)this, a); }
		Vector(int repeat) : X(repeat), Y(repeat), Z(repeat) { }
		explicit Vector(const int* arr) : X(arr[0]), Y(arr[1]), Z(arr[2]) { }
		Vector(int x, int y, int z) : X(x), Y(y), Z(z) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; Z = rhs.Z; return *this; }
		inline Vector& __vectorcall operator=(const AlignedType& rhs) { XMStoreInt3((uint32_t*)this, rhs); return *this; }

		VectorIntMembers(3, Unaligned)

			inline operator XMVECTOR() const { return XMLoadInt3((uint32_t*)this); }

		static const Vector Zero;
		static const Vector One;
		static const Vector NegativeOne;
	};

	template<>
	struct Vector<int, 4, true> : public VectorBaseTraits<int, 4>
	{
		VectorAlignedIntConstructors(4)
			Vector(int x, int y, int z, int w) : _xm(XMVectorSetInt(x, y, z, w)) {}
		inline Vector& __vectorcall operator=(const Vector& rhs) { _xm = rhs; return *this; }
		//inline Vector& __vectorcall operator=(const UnalignedType& rhs) { _xm = rhs; return *this; }

		VectorIntMembers(4, Aligned)

			static const Vector Zero;
		static const Vector One;
		static const Vector NegativeOne;
	};

	template<>
	struct Vector<int, 4, false> : public VectorBaseTraits<int, 4>
	{
		Vector() { }
		Vector(XMVECTOR xm) { XMStoreInt4((uint32_t*)this, xm); }
		Vector(const AlignedType& a) { XMStoreInt4((uint32_t*)this, a); }
		Vector(int repeat) : X(repeat), Y(repeat), Z(repeat), W(repeat) { }
		explicit Vector(const int* arr) : X(arr[0]), Y(arr[1]), Z(arr[2]), W(arr[3]) { }
		Vector(int x, int y, int z, int w) : X(x), Y(y), Z(z), W(w) { }
		inline Vector& __vectorcall operator=(const Vector& rhs) { X = rhs.X; Y = rhs.Y; Z = rhs.Z; W = rhs.W; return *this; }
		inline Vector& __vectorcall operator=(const AlignedType& rhs) { XMStoreInt4((uint32_t*)this, rhs); return *this; }

		VectorIntMembers(4, Unaligned)

			inline operator XMVECTOR() const { return XMLoadInt4((uint32_t*)this); }

		static const Vector Zero;
		static const Vector One;
		static const Vector NegativeOne;
	};

	typedef Vector<float, 2, false> Float2;
	typedef Vector<float, 2, true> Float2A;
	typedef Vector<float, 3, false> Float3;
	typedef Vector<float, 3, true> Float3A;
	typedef Vector<float, 4, false> Float4;
	typedef Vector<float, 4, true> Float4A;
	typedef Vector<int, 2, false> Int2;
	typedef Vector<int, 2, true> Int2A;
	typedef Vector<int, 3, false> Int3;
	typedef Vector<int, 3, true> Int3A;
	typedef Vector<int, 4, false> Int4;
	typedef Vector<int, 4, true> Int4A;
	typedef Vector<bool, 2, false> Bool2;
	typedef Vector<bool, 2, true> Bool2A;
	typedef Vector<bool, 3, false> Bool3;
	typedef Vector<bool, 3, true> Bool3A;
	typedef Vector<bool, 4, false> Bool4;
	typedef Vector<bool, 4, true> Bool4A;

	struct Quaternion;
	struct QuaternionA
	{
	protected:
		XMVECTOR _xm;

	public:
		typedef QuaternionA AlignedType;
		typedef Quaternion UnalignedType;

		static const QuaternionA Zero;
		static const QuaternionA One;
		static const QuaternionA Identity;

		QuaternionA() { }
		QuaternionA(float x, float y, float z, float w) : _xm(XMVectorSet(x, y, z, w)) { }
		QuaternionA(XMVECTOR xm) : _xm(xm) { }
		explicit QuaternionA(const Float4A& v) : _xm(v) { }
		QuaternionA(const Quaternion& ua);
		inline QuaternionA& __vectorcall operator=(const QuaternionA& rhs) { _xm = rhs; return *this; }
		//inline QuaternionA& __vectorcall operator=(const Quaternion& rhs) { _xm = rhs; return *this; }

		PROPERTY(float, X);
		float GetX() const { return XMVectorGetX(_xm); }
		void SetX(float x) { _xm = XMVectorSetX(_xm, x); }

		PROPERTY(float, Y);
		float GetY() const { return XMVectorGetY(_xm); }
		void SetY(float y) { _xm = XMVectorSetY(_xm, y); }

		PROPERTY(float, Z);
		float GetZ() const { return XMVectorGetZ(_xm); }
		void SetZ(float z) { _xm = XMVectorSetZ(_xm, z); }

		PROPERTY(float, W);
		float GetW() const { return XMVectorGetW(_xm); }
		void SetW(float w) { _xm = XMVectorSetW(_xm, w); }

		inline operator XMVECTOR() const { return _xm; }

		static inline QuaternionA __vectorcall Slerp(const QuaternionA& q1, const QuaternionA& q2, float u) { return XMQuaternionSlerp(q1, q2, u); }
		static inline QuaternionA __vectorcall SlerpV(const QuaternionA& q1, const QuaternionA& q2, const Float4A& u) { return XMQuaternionSlerpV(q1, q2, u); }
		static inline QuaternionA __vectorcall Barycentric(const QuaternionA& q1, const QuaternionA& q2, const QuaternionA& q3, float f, float g) { return XMQuaternionBaryCentric(q1, q2, q3, f, g); }
		static inline QuaternionA __vectorcall BarycentricV(const QuaternionA& q1, const QuaternionA& q2, const QuaternionA& q3, const Float4A& f, const Float4A& g) { return XMQuaternionBaryCentricV(q1, q2, q3, f, g); }

		static inline QuaternionA __vectorcall FromPitchYawRoll(float pitch, float yaw, float roll) { return XMQuaternionRotationRollPitchYaw(pitch, yaw, roll); }
		static inline QuaternionA __vectorcall FromPitchYawRoll(const Float3A& pitchYawRoll) { return XMQuaternionRotationRollPitchYawFromVector(pitchYawRoll); }
		static inline QuaternionA __vectorcall FromAxisAngle(const Float3A& axis, float angle) { return XMQuaternionRotationAxis(axis, angle); }
		static inline QuaternionA __vectorcall FromNormalAxisAngle(const Float3A& axis, float angle) { return XMQuaternionRotationNormal(axis, angle); }

		inline bool __vectorcall operator ==(const QuaternionA& rhs) const { return XMQuaternionEqual(*this, rhs); }
		inline bool __vectorcall operator !=(const QuaternionA& rhs) const { return XMQuaternionNotEqual(*this, rhs); }

		PROPERTY_READONLY(bool, IsNan);
		inline bool __vectorcall GetIsNan() const { return XMQuaternionIsNaN(*this); }

		PROPERTY_READONLY(bool, IsInfinite);
		inline bool __vectorcall GetIsInfinite() const { return XMQuaternionIsInfinite(*this); }

		PROPERTY_READONLY(bool, IsIdentity);
		inline bool __vectorcall GetIsIdentity() const { return XMQuaternionIsIdentity(*this); }

		PROPERTY_READONLY(float, LengthSquared);
		inline float __vectorcall GetLengthSquared() const { return XMVectorGetX(XMQuaternionLengthSq(*this)); }
		inline Float4A __vectorcall SplatLengthSquared() const { return XMQuaternionLengthSq(*this); }

		PROPERTY_READONLY(float, ReciprocalLength);
		inline float __vectorcall GetReciprocalLength() const { return XMVectorGetX(XMQuaternionReciprocalLength(*this)); }
		inline Float4A __vectorcall SplatReciprocalLength() const { return XMQuaternionReciprocalLength(*this); }

		PROPERTY_READONLY(float, Length);
		inline float __vectorcall GetLength() const { return XMVectorGetX(XMQuaternionLength(*this)); }
		inline Float4A __vectorcall SplatLength() const { return XMQuaternionLength(*this); }

		inline QuaternionA __vectorcall Dot(const QuaternionA& rhs) const { return XMQuaternionDot(*this, rhs); }

		inline QuaternionA __vectorcall Normalize() const { return XMQuaternionNormalize(*this); }

		PROPERTY_READONLY(QuaternionA, Conjugate);
		inline QuaternionA __vectorcall GetConjugate() const { return XMQuaternionConjugate(*this); }

		PROPERTY_READONLY(QuaternionA, Inverse);
		inline QuaternionA __vectorcall GetInverse() const { return XMQuaternionInverse(*this); }

		inline QuaternionA __vectorcall Ln() const { return XMQuaternionLn(*this); }
		inline QuaternionA __vectorcall Exp() const { return XMQuaternionExp(*this); }

		inline void __vectorcall ToAxisAngle(Float3A& outAxis, float& outAngle) const { XMQuaternionToAxisAngle((XMVECTOR*)&outAxis, &outAngle, *this); }
		inline void __vectorcall ToAxisAngle(Float3& outAxis, float& outAngle) const { XMVECTOR oa; XMQuaternionToAxisAngle(&oa, &outAngle, *this); outAxis = (Float3A)oa; }

		inline QuaternionA __vectorcall operator*(const QuaternionA& rhs) const { return XMQuaternionMultiply(*this, rhs); }
		inline QuaternionA& __vectorcall operator*=(const QuaternionA& rhs) { _xm = XMQuaternionMultiply(*this, rhs); return *this; }

		struct _Estimate
		{
			const XMVECTOR& _q;
			_Estimate(const QuaternionA& q) : _q(q) { }

			inline QuaternionA __vectorcall Normalize() { return XMQuaternionNormalizeEst(_q); }
		};

		PROPERTY_READONLY(_Estimate, Estimate);
		_Estimate GetEstimate() { return _Estimate(*this); }
	};

	struct Quaternion
	{
		typedef QuaternionA AlignedType;
		typedef Quaternion UnalignedType;

		float X, Y, Z, W;

		static const Quaternion Zero;
		static const Quaternion One;
		static const Quaternion Identity;

		Quaternion() { }
		Quaternion(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) { }
		explicit Quaternion(Float4 v) : X(v.X), Y(v.Y), Z(v.Z), W(v.W) { }
		explicit Quaternion(const Float4A& v) : X(v.X), Y(v.Y), Z(v.Z), W(v.W) { }
		Quaternion(const QuaternionA& q) : X(q.X), Y(q.Y), Z(q.Z), W(q.W) { }

		inline operator XMVECTOR() const { return XMVectorSet(X, Y, Z, W); }
		inline Quaternion& __vectorcall operator=(const QuaternionA& rhs) { XMStoreFloat4((XMFLOAT4*)this, rhs); return *this; }

		static inline QuaternionA __vectorcall Slerp(const QuaternionA& q1, const QuaternionA& q2, float u) { return XMQuaternionSlerp(q1, q2, u); }
		static inline QuaternionA __vectorcall SlerpV(const QuaternionA& q1, const QuaternionA& q2, const Float4A& u) { return XMQuaternionSlerpV(q1, q2, u); }
		static inline QuaternionA __vectorcall Barycentric(const QuaternionA& q1, const QuaternionA& q2, const QuaternionA& q3, float f, float g) { return XMQuaternionBaryCentric(q1, q2, q3, f, g); }
		static inline QuaternionA __vectorcall BarycentricV(const QuaternionA& q1, const QuaternionA& q2, const QuaternionA& q3, const Float4A& f, const Float4A& g) { return XMQuaternionBaryCentricV(q1, q2, q3, f, g); }

		static inline QuaternionA __vectorcall FromPitchYawRoll(float pitch, float yaw, float roll) { return XMQuaternionRotationRollPitchYaw(pitch, yaw, roll); }
		static inline QuaternionA __vectorcall FromPitchYawRoll(const Float3A& pitchYawRoll) { return XMQuaternionRotationRollPitchYawFromVector(pitchYawRoll); }
		static inline QuaternionA __vectorcall FromAxisAngle(const Float3A& axis, float angle) { return XMQuaternionRotationAxis(axis, angle); }
		static inline QuaternionA __vectorcall FromNormalAxisAngle(const Float3A& axis, float angle) { return XMQuaternionRotationNormal(axis, angle); }

		inline bool __vectorcall operator ==(const QuaternionA& rhs) const { return XMQuaternionEqual(*this, rhs); }
		inline bool __vectorcall operator !=(const QuaternionA& rhs) const { return XMQuaternionNotEqual(*this, rhs); }

		PROPERTY_READONLY(bool, IsNan);
		inline bool __vectorcall GetIsNan() const { return XMQuaternionIsNaN(*this); }

		PROPERTY_READONLY(bool, IsInfinite);
		inline bool __vectorcall GetIsInfinite() const { return XMQuaternionIsInfinite(*this); }

		PROPERTY_READONLY(bool, IsIdentity);
		inline bool __vectorcall GetIsIdentity() const { return XMQuaternionIsIdentity(*this); }

		PROPERTY_READONLY(float, LengthSquared);
		inline float __vectorcall GetLengthSquared() const { return XMVectorGetX(XMQuaternionLengthSq(*this)); }
		inline Float4A __vectorcall SplatLengthSquared() const { return XMQuaternionLengthSq(*this); }

		PROPERTY_READONLY(float, ReciprocalLength);
		inline float __vectorcall GetReciprocalLength() const { return XMVectorGetX(XMQuaternionReciprocalLength(*this)); }
		inline Float4A __vectorcall SplatReciprocalLength() const { return XMQuaternionReciprocalLength(*this); }

		PROPERTY_READONLY(float, Length);
		inline float __vectorcall GetLength() const { return XMVectorGetX(XMQuaternionLength(*this)); }
		inline Float4A __vectorcall SplatLength() const { return XMQuaternionLength(*this); }

		inline QuaternionA __vectorcall Dot(const QuaternionA& rhs) const { return XMQuaternionDot(*this, rhs); }

		inline QuaternionA __vectorcall Normalize() const { return XMQuaternionNormalize(*this); }

		PROPERTY_READONLY(QuaternionA, Conjugate);
		inline QuaternionA __vectorcall GetConjugate() const { return XMQuaternionConjugate(*this); }

		PROPERTY_READONLY(QuaternionA, Inverse);
		inline QuaternionA __vectorcall GetInverse() const { return XMQuaternionInverse(*this); }

		inline QuaternionA __vectorcall Ln() const { return XMQuaternionLn(*this); }
		inline QuaternionA __vectorcall Exp() const { return XMQuaternionExp(*this); }

		inline void __vectorcall ToAxisAngle(Float3A& outAxis, float& outAngle) const { XMQuaternionToAxisAngle((XMVECTOR*)&outAxis, &outAngle, *this); }
		inline void __vectorcall ToAxisAngle(Float3& outAxis, float& outAngle) const { XMVECTOR oa; XMQuaternionToAxisAngle(&oa, &outAngle, *this); outAxis = (Float3A)oa; }

		inline QuaternionA __vectorcall operator*(const QuaternionA& rhs) const { return XMQuaternionMultiply(*this, rhs); }
		inline Quaternion& __vectorcall operator*=(const QuaternionA& rhs) { *this = XMQuaternionMultiply(*this, rhs); return *this; }

		struct _Estimate
		{
			const XMVECTOR& _q;
			_Estimate(const QuaternionA& q) : _q(q) { }

			inline QuaternionA __vectorcall Normalize() { return XMQuaternionNormalizeEst(_q); }
		};

		PROPERTY_READONLY(_Estimate, Estimate);
		_Estimate GetEstimate() { return _Estimate(*this); }
	};

	inline Float3A __vectorcall Float3A::Rotate(const QuaternionA& q) const { return XMVector3Rotate(*this, q); }
	inline Float3A __vectorcall Float3::Rotate(const QuaternionA& q) const { return XMVector3Rotate(*this, q); }

	struct Float4x4A
	{
	protected:
		XMMATRIX _xm;

	public:
		typedef Float4x4A AlignedType;
		typedef Float4x4 UnalignedType;

		static const Float4x4A Identity;
		static const Float4x4A Zero;
		static const Float4x4A One;

		Float4x4A() { }
		Float4x4A(const XMMATRIX& xm) : _xm(xm) { }
		Float4x4A(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33) :
			_xm(XMMatrixSet(
				m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33)) { }
		explicit Float4x4A(const float* arr) : _xm(XMLoadFloat4x4((XMFLOAT4X4*)arr)) { }
		Float4x4A(const Float4x4& copy);
		Float4x4A(const Float4x3& copy);
		Float4x4A(const Float4x3A& copy);

		inline operator XMMATRIX() const { return _xm; }

		PROPERTY(float, _11);
		inline float __vectorcall Get_11() const { return XMVectorGetX(_xm.r[0]); }
		inline void __vectorcall Set_11(float x) { _xm.r[0] = XMVectorSetX(_xm.r[0], x); }

		PROPERTY(float, _12);
		inline float __vectorcall Get_12() const { return XMVectorGetY(_xm.r[0]); }
		inline void __vectorcall Set_12(float x) { _xm.r[0] = XMVectorSetY(_xm.r[0], x); }

		PROPERTY(float, _13);
		inline float __vectorcall Get_13() const { return XMVectorGetZ(_xm.r[0]); }
		inline void __vectorcall Set_13(float x) { _xm.r[0] = XMVectorSetZ(_xm.r[0], x); }

		PROPERTY(float, _14);
		inline float __vectorcall Get_14() const { return XMVectorGetW(_xm.r[0]); }
		inline void __vectorcall Set_14(float x) { _xm.r[0] = XMVectorSetW(_xm.r[1], x); }

		PROPERTY(float, _21);
		inline float __vectorcall Get_21() const { return XMVectorGetX(_xm.r[1]); }
		inline void __vectorcall Set_21(float x) { _xm.r[1] = XMVectorSetX(_xm.r[1], x); }

		PROPERTY(float, _22);
		inline float __vectorcall Get_22() const { return XMVectorGetY(_xm.r[1]); }
		inline void __vectorcall Set_22(float x) { _xm.r[1] = XMVectorSetY(_xm.r[1], x); }

		PROPERTY(float, _23);
		inline float __vectorcall Get_23() const { return XMVectorGetZ(_xm.r[1]); }
		inline void __vectorcall Set_23(float x) { _xm.r[1] = XMVectorSetZ(_xm.r[1], x); }

		PROPERTY(float, _24);
		inline float __vectorcall Get_24() const { return XMVectorGetW(_xm.r[1]); }
		inline void __vectorcall Set_24(float x) { _xm.r[1] = XMVectorSetW(_xm.r[1], x); }

		PROPERTY(float, _31);
		inline float __vectorcall Get_31() const { return XMVectorGetX(_xm.r[2]); }
		inline void __vectorcall Set_31(float x) { _xm.r[2] = XMVectorSetX(_xm.r[2], x); }

		PROPERTY(float, _32);
		inline float __vectorcall Get_32() const { return XMVectorGetY(_xm.r[2]); }
		inline void __vectorcall Set_32(float x) { _xm.r[2] = XMVectorSetY(_xm.r[2], x); }

		PROPERTY(float, _33);
		inline float __vectorcall Get_33() const { return XMVectorGetZ(_xm.r[2]); }
		inline void __vectorcall Set_33(float x) { _xm.r[2] = XMVectorSetZ(_xm.r[2], x); }

		PROPERTY(float, _34);
		inline float __vectorcall Get_34() const { return XMVectorGetW(_xm.r[2]); }
		inline void __vectorcall Set_34(float x) { _xm.r[2] = XMVectorSetW(_xm.r[2], x); }

		PROPERTY(float, _41);
		inline float __vectorcall Get_41() const { return XMVectorGetX(_xm.r[3]); }
		inline void __vectorcall Set_41(float x) { _xm.r[3] = XMVectorSetX(_xm.r[3], x); }

		PROPERTY(float, _42);
		inline float __vectorcall Get_42() const { return XMVectorGetY(_xm.r[3]); }
		inline void __vectorcall Set_42(float x) { _xm.r[3] = XMVectorSetY(_xm.r[3], x); }

		PROPERTY(float, _43);
		inline float __vectorcall Get_43() const { return XMVectorGetZ(_xm.r[3]); }
		inline void __vectorcall Set_43(float x) { _xm.r[3] = XMVectorSetZ(_xm.r[3], x); }

		PROPERTY(float, _44);
		inline float __vectorcall Get_44() const { return XMVectorGetW(_xm.r[3]); }
		inline void __vectorcall Set_44(float x) { _xm.r[3] = XMVectorSetW(_xm.r[3], x); }

		PROPERTY_INDEXABLE(Float4A, Row);
		inline Float4A __vectorcall GetRow(int i) const { assert(i >= 0 && i < 4); return _xm.r[i]; }
		inline void __vectorcall SetRow(int i, const Float4A& value) { assert(i >= 0 && i < 4); _xm.r[i] = value; }

		PROPERTY_INDEXABLE(Float4A, Column);
		inline Float4A GetColumn(int j) const {
			assert(j >= 0 && j < 4); return Float4A(
				XMVectorGetByIndex(_xm.r[0], j),
				XMVectorGetByIndex(_xm.r[1], j),
				XMVectorGetByIndex(_xm.r[2], j),
				XMVectorGetByIndex(_xm.r[3], j));
		}
		inline void SetColumn(int j, Float4 value)
		{
			assert(j >= 0 && j < 4);
			XMVectorSetByIndex(_xm.r[0], value.X, j);
			XMVectorSetByIndex(_xm.r[1], value.Y, j);
			XMVectorSetByIndex(_xm.r[2], value.Z, j);
			XMVectorSetByIndex(_xm.r[3], value.W, j);
		}

		inline Float4x4A& operator =(const Float4x4& rhs);
		inline Float4x4A& operator =(const XMMATRIX& xm) { _xm = xm; return *this; }
		inline Float4x4A& __vectorcall operator =(const Float4x4A& rhs) { _xm = rhs._xm; return *this; }
		inline Float4x4A __vectorcall operator +(const Float4x4A& rhs) const { return _xm + rhs._xm; }
		inline Float4x4A __vectorcall operator *(const Float4x4A& rhs) const { return _xm * rhs._xm; }
		inline Float4x4A __vectorcall operator -(const Float4x4A& rhs) const { return _xm - rhs._xm; }
		inline Float4x3A __vectorcall operator *(const Float4x3A& rhs) const;
		inline Float4x4A __vectorcall operator *(float scalar) const { return _xm * scalar; }
		inline Float4x4A __vectorcall operator /(float scalar) const { return _xm / scalar; }
		inline Float4x4A& __vectorcall operator +=(const Float4x4A& rhs) { _xm = _xm + rhs; return *this; }
		inline Float4x4A& __vectorcall operator -=(const Float4x4A& rhs) { _xm = _xm - rhs; return *this; }
		inline Float4x4A& __vectorcall operator *=(const Float4x4A& rhs) { _xm = _xm * rhs; return *this; }
		inline Float4x4A& __vectorcall operator *=(float scalar) { _xm = _xm * scalar; return *this; }
		inline Float4x4A& __vectorcall operator /=(float scalar) { _xm = _xm / scalar; return *this; }
		inline bool __vectorcall operator ==(const Float4x4A& r) const throw()
		{
			return XMVector4Equal(_xm.r[0], r._xm.r[0]) &&
				XMVector4Equal(_xm.r[0], r._xm.r[0]) &&
				XMVector4Equal(_xm.r[0], r._xm.r[0]) &&
				XMVector4Equal(_xm.r[0], r._xm.r[0]);
		}
		inline bool __vectorcall operator !=(const Float4x4A& r) const throw()
		{
			return XMVector4NotEqual(_xm.r[0], r._xm.r[0]) ||
				XMVector4NotEqual(_xm.r[0], r._xm.r[0]) ||
				XMVector4NotEqual(_xm.r[0], r._xm.r[0]) ||
				XMVector4NotEqual(_xm.r[0], r._xm.r[0]);
		}

		PROPERTY_READONLY(bool, IsNan);
		inline bool __vectorcall GetIsNan() const { return XMMatrixIsNaN(*this); }

		PROPERTY_READONLY(bool, IsInfinite);
		inline bool __vectorcall GetIsInfinite() const { return XMMatrixIsInfinite(*this); }

		PROPERTY_READONLY(bool, IsIdentity);
		inline bool __vectorcall GetIsIdentity() const { return XMMatrixIsIdentity(*this); }

		PROPERTY_READONLY(Float4x4A, Inverse);
		inline Float4x4A __vectorcall GetInverse() const { return XMMatrixInverse(nullptr, *this); }

		inline Float4x4A __vectorcall InverseWithHint(const Float4A& determinantSplat) { return XMMatrixInverse(&(XMVECTOR)determinantSplat, *this); }

		PROPERTY_READONLY(Float4x4A, Transpose);
		inline Float4x4A __vectorcall GetTranspose() const { return XMMatrixTranspose(*this); }

		PROPERTY_READONLY(float, Determinant);
		inline float __vectorcall GetDeterminant() const { return XMVectorGetX(XMMatrixDeterminant(*this)); }

		inline Float4A __vectorcall SplatDeterminant() const { return XMMatrixDeterminant(*this); }

		inline Float4x4A __vectorcall MultiplyThenTranspose(const Float4x4A& m) const { return XMMatrixMultiplyTranspose(*this, m); }
		inline Float4x4A __vectorcall MultiplyThenTranspose(const Float4x3A& m) const;

		bool Decompose(Float3* outPosition, Quaternion* outOrientation, Float3* outScale, Float4x4A* outResidual = nullptr) const;

		inline static Float4x4A __vectorcall Translation(float x, float y, float z) { return XMMatrixTranslation(x, y, z); }
		inline static Float4x4A __vectorcall Translation(const Float3A& v) { return XMMatrixTranslationFromVector(v); }
		inline static Float4x4A __vectorcall Scaling(float x, float y, float z) { return XMMatrixScaling(x, y, z); }
		inline static Float4x4A __vectorcall Scaling(const Float3A& v) { return XMMatrixScalingFromVector(v); }
		inline static Float4x4A __vectorcall RotationX(float angle) { return XMMatrixRotationX(angle); }
		inline static Float4x4A __vectorcall RotationY(float angle) { return XMMatrixRotationY(angle); }
		inline static Float4x4A __vectorcall RotationZ(float angle) { return XMMatrixRotationZ(angle); }
		inline static Float4x4A __vectorcall RotationPitchYawRoll(float pitch, float yaw, float roll) { return XMMatrixRotationRollPitchYaw(pitch, yaw, roll); }
		inline static Float4x4A __vectorcall RotationPitchYawRoll(const Float3A& v) { return XMMatrixRotationRollPitchYawFromVector(v); }
		inline static Float4x4A __vectorcall RotationNormalAxisAngle(const Float3A& normalAxis, float angle) { return XMMatrixRotationNormal(normalAxis, angle); }
		inline static Float4x4A __vectorcall RotationAxisAngle(const Float3A& axis, float angle) { return XMMatrixRotationAxis(axis, angle); }
		inline static Float4x4A __vectorcall RotationQuaternion(const QuaternionA& q) { return XMMatrixRotationQuaternion(q); }
		inline static Float4x4A __vectorcall Transform2D(const Float2A& scale, const Float2A& rotationCenter, float rotationAngle, const Float2A& position) { return XMMatrixAffineTransformation2D(scale, rotationCenter, rotationAngle, position); }
		inline static Float4x4A __vectorcall Transform(const Float3A& scale, const Float3A& rotationCenter, const QuaternionA& rotation, const Float3A& position) { return XMMatrixAffineTransformation(scale, rotationCenter, rotation, position); }
		inline static Float4x4A __vectorcall Reflection(const Float4A& reflectionPlane) { return XMMatrixReflect(reflectionPlane); }
		inline static Float4x4A __vectorcall Shadow(const Float3A& lightPosition, const Float4A& shadowPlane) { return XMMatrixShadow(shadowPlane, lightPosition); }
		inline static Float4x4A __vectorcall LookAt(const Float3A& eyePosition, const Float3A& focusPt, const Float3A& up) { return XMMatrixLookAtLH(eyePosition, focusPt, up); }
		inline static Float4x4A __vectorcall LookTo(const Float3A& eyePosition, const Float3A& eyeDirection, const Float3A& up) { return XMMatrixLookToLH(eyePosition, eyeDirection, up); }

		inline static Float4x4A __vectorcall Perspective(float viewWidth, float viewHeight, float nearZ, float farZ) { return XMMatrixPerspectiveLH(viewWidth, viewHeight, nearZ, farZ); }
		inline static Float4x4A __vectorcall PerspectiveFov(float fovAngleY, float aspectRatio, float nearZ, float farZ) { return XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ); }
		inline static Float4x4A __vectorcall PerspectiveOffCenter(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ) { return XMMatrixPerspectiveOffCenterLH(viewLeft, viewRight, viewBottom, viewTop, nearZ, farZ); }
		inline static Float4x4A __vectorcall Orthographic(float viewWidth, float viewHeight, float nearZ, float farZ) { return XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ); }
		inline static Float4x4A __vectorcall OrthographicOffCenter(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ) { return XMMatrixOrthographicOffCenterLH(viewLeft, viewRight, viewBottom, viewTop, nearZ, farZ); }
	};

	struct Float4x4
	{
		typedef Float4x4A AlignedType;
		typedef Float4x4 UnalignedType;

		static const Float4x4 Identity;
		static const Float4x4 Zero;
		static const Float4x4 One;

		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float m[4][4];
		};

		Float4x4() { }
		Float4x4(
			float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33) :
			_11(m00), _12(m01), _13(m02), _14(m03),
			_21(m10), _22(m11), _23(m12), _24(m13),
			_31(m20), _32(m21), _33(m22), _34(m23),
			_41(m30), _42(m31), _43(m32), _44(m33) { }
		explicit Float4x4(const float* arr) :
			_11(arr[0]), _12(arr[1]), _13(arr[2]), _14(arr[3]),
			_21(arr[4]), _22(arr[5]), _23(arr[6]), _24(arr[7]),
			_31(arr[8]), _32(arr[9]), _33(arr[10]), _34(arr[11]),
			_41(arr[12]), _42(arr[13]), _43(arr[14]), _44(arr[15]) { }
		Float4x4(const XMMATRIX& copy) { XMStoreFloat4x4((XMFLOAT4X4*)this, copy); }
		Float4x4(const Float4x4A& copy) { XMStoreFloat4x4((XMFLOAT4X4*)this, copy); }
		Float4x4(const Float4x3A& copy);
		Float4x4(const Float4x3& copy);

		inline operator XMMATRIX() const { return XMLoadFloat4x4((XMFLOAT4X4*)this); }

		PROPERTY_INDEXABLE(Float4, Row);
		inline Float4 GetRow(int i) const { assert(i >= 0 && i < 4); return *(Float4*)m[i]; }
		inline void SetRow(int i, Float4 value) { assert(i >= 0 && i < 4); *(Float4*)m[i] = value; }

		PROPERTY_INDEXABLE(Float4, Column);
		inline Float4 GetColumn(int j) const {
			assert(j >= 0 && j < 4); return Float4(m[0][j], m[1][j], m[2][j], m[3][j]);
		}
		inline void SetColumn(int j, Float4 value)
		{
			assert(j >= 0 && j < 4);
			m[0][j] = value.X;
			m[1][j] = value.Y;
			m[2][j] = value.Z;
			m[3][j] = value.W;
		}

		inline Float4x4& operator =(const Float4x4& rhs) { memcpy((void*)m, (void*)rhs.m, sizeof(float) * 16); return *this; }
		inline Float4x4& __vectorcall operator =(const Float4x4A& rhs) { XMStoreFloat4x4((XMFLOAT4X4*)this, rhs); return *this; }
		inline Float4x4A __vectorcall operator +(const Float4x4A& rhs) const { return (XMMATRIX)*this + (XMMATRIX)rhs; }
		inline Float4x4A __vectorcall operator *(const Float4x4A& rhs) const { return (XMMATRIX)*this * (XMMATRIX)rhs; }
		inline Float4x4A __vectorcall operator -(const Float4x4A& rhs) const { return (XMMATRIX)*this - (XMMATRIX)rhs; }
		inline Float4x3A __vectorcall operator *(const Float4x3A& rhs) const;
		inline Float4x4A __vectorcall operator *(float scalar) const { return (XMMATRIX)*this * scalar; }
		inline Float4x4A __vectorcall operator /(float scalar) const { return (XMMATRIX)*this / scalar; }
		inline Float4x4& __vectorcall operator +=(const Float4x4A& rhs) { XMStoreFloat4x4((XMFLOAT4X4*)this, (XMMATRIX)*this + rhs); return *this; }
		inline Float4x4& __vectorcall operator -=(const Float4x4A& rhs) { XMStoreFloat4x4((XMFLOAT4X4*)this, (XMMATRIX)*this - rhs); return *this; }
		inline Float4x4& __vectorcall operator *=(const Float4x4A& rhs) { XMStoreFloat4x4((XMFLOAT4X4*)this, (XMMATRIX)*this * rhs); return *this; }
		inline Float4x4& __vectorcall operator *=(float scalar) { XMStoreFloat4x4((XMFLOAT4X4*)this, (XMMATRIX)*this * scalar); return *this; }
		inline Float4x4& __vectorcall operator /=(float scalar) { XMStoreFloat4x4((XMFLOAT4X4*)this, (XMMATRIX)*this / scalar); return *this; }
		bool operator ==(const Float4x4& r) const throw();
		bool operator !=(const Float4x4& r) const throw();

		PROPERTY_READONLY(bool, IsNan);
		inline bool __vectorcall GetIsNan() const { return XMMatrixIsNaN(*this); }

		PROPERTY_READONLY(bool, IsInfinite);
		inline bool __vectorcall GetIsInfinite() const { return XMMatrixIsInfinite(*this); }

		PROPERTY_READONLY(bool, IsIdentity);
		inline bool __vectorcall GetIsIdentity() const { return XMMatrixIsIdentity(*this); }

		PROPERTY_READONLY(Float4x4A, Inverse);
		inline Float4x4A __vectorcall GetInverse() const { return XMMatrixInverse(nullptr, *this); }

		inline Float4x4A __vectorcall InverseWithHint(const Float4A& determinantSplat) { return XMMatrixInverse(&(XMVECTOR)determinantSplat, *this); }

		PROPERTY_READONLY(Float4x4A, Transpose);
		inline Float4x4A __vectorcall GetTranspose() const { return XMMatrixTranspose(*this); }

		PROPERTY_READONLY(float, Determinant);
		inline float __vectorcall GetDeterminant() const { return XMVectorGetX(XMMatrixDeterminant(*this)); }

		inline Float4A __vectorcall SplatDeterminant() const { return XMMatrixDeterminant(*this); }

		inline Float4x4A __vectorcall MultiplyThenTranspose(const Float4x4A& m) const { return XMMatrixMultiplyTranspose(*this, m); }
		inline Float4x4A __vectorcall MultiplyThenTranspose(const Float4x3A& m) const;

		bool Decompose(Float3* outPosition, Quaternion* outOrientation, Float3* outScale, Float4x4A* outResidual = nullptr) const;

		inline static Float4x4 __vectorcall Translation(float x, float y, float z) { return XMMatrixTranslation(x, y, z); }
		inline static Float4x4 __vectorcall Translation(const Float3A& v) { return XMMatrixTranslationFromVector(v); }
		inline static Float4x4 __vectorcall Scaling(float x, float y, float z) { return XMMatrixScaling(x, y, z); }
		inline static Float4x4 __vectorcall Scaling(const Float3A& v) { return XMMatrixScalingFromVector(v); }
		inline static Float4x4 __vectorcall RotationX(float angle) { return XMMatrixRotationX(angle); }
		inline static Float4x4 __vectorcall RotationY(float angle) { return XMMatrixRotationY(angle); }
		inline static Float4x4 __vectorcall RotationZ(float angle) { return XMMatrixRotationZ(angle); }
		inline static Float4x4 __vectorcall RotationPitchYawRoll(float pitch, float yaw, float roll) { return XMMatrixRotationRollPitchYaw(pitch, yaw, roll); }
		inline static Float4x4 __vectorcall RotationPitchYawRoll(const Float3A& v) { return XMMatrixRotationRollPitchYawFromVector(v); }
		inline static Float4x4 __vectorcall RotationNormalAxisAngle(const Float3A& normalAxis, float angle) { return XMMatrixRotationNormal(normalAxis, angle); }
		inline static Float4x4 __vectorcall RotationAxisAngle(const Float3A& axis, float angle) { return XMMatrixRotationAxis(axis, angle); }
		inline static Float4x4 __vectorcall RotationQuaternion(const QuaternionA& q) { return XMMatrixRotationQuaternion(q); }
		inline static Float4x4 __vectorcall Transform2D(const Float2A& scale, const Float2A& rotationCenter, float rotationAngle, const Float2A& position) { return XMMatrixAffineTransformation2D(scale, rotationCenter, rotationAngle, position); }
		inline static Float4x4 __vectorcall Transform(const Float3A& scale, const Float3A& rotationCenter, const QuaternionA& rotation, const Float3A& position) { return XMMatrixAffineTransformation(scale, rotationCenter, rotation, position); }
		inline static Float4x4 __vectorcall Reflection(const Float4A& reflectionPlane) { return XMMatrixReflect(reflectionPlane); }
		inline static Float4x4 __vectorcall Shadow(const Float3A& lightPosition, const Float4A& shadowPlane) { return XMMatrixShadow(shadowPlane, lightPosition); }
		inline static Float4x4 __vectorcall LookAt(const Float3A& eyePosition, const Float3A& focusPt, const Float3A& up) { return XMMatrixLookAtLH(eyePosition, focusPt, up); }
		inline static Float4x4 __vectorcall LookTo(const Float3A& eyePosition, const Float3A& eyeDirection, const Float3A& up) { return XMMatrixLookToLH(eyePosition, eyeDirection, up); }

		inline static Float4x4 __vectorcall Perspective(float viewWidth, float viewHeight, float nearZ, float farZ) { return XMMatrixPerspectiveLH(viewWidth, viewHeight, nearZ, farZ); }
		inline static Float4x4 __vectorcall PerspectiveFov(float fovAngleY, float aspectRatio, float nearZ, float farZ) { return XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ); }
		inline static Float4x4 __vectorcall PerspectiveOffCenter(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ) { return XMMatrixPerspectiveOffCenterLH(viewLeft, viewRight, viewBottom, viewTop, nearZ, farZ); }
		inline static Float4x4 __vectorcall Orthographic(float viewWidth, float viewHeight, float nearZ, float farZ) { return XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ); }
		inline static Float4x4 __vectorcall OrthographicOffCenter(float viewLeft, float viewRight, float viewBottom, float viewTop, float nearZ, float farZ) { return XMMatrixOrthographicOffCenterLH(viewLeft, viewRight, viewBottom, viewTop, nearZ, farZ); }
	};

	struct Float4x3A
	{
	protected:
		XMMATRIX _xm;

	public:
		typedef Float4x3A AlignedType;
		typedef Float4x3 UnalignedType;

		static const Float4x3A Identity;
		static const Float4x3A Zero;
		static const Float4x3A One;

		Float4x3A() { }
		Float4x3A(const XMMATRIX& xm) : _xm(xm) { }
		Float4x3A(
			float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22,
			float m30, float m31, float m32) :
			_xm(XMMatrixSet(
				m00, m01, m02, 0,
				m10, m11, m12, 0,
				m20, m21, m22, 0,
				m30, m31, m32, 1)) { }
		explicit Float4x3A(const float* arr) : _xm(XMLoadFloat4x3((XMFLOAT4X3*)arr)) { }
		Float4x3A(const Float4x3& copy);

		inline operator XMMATRIX() const { return _xm; }

		PROPERTY_INDEXABLE(Float3A, Row);
		inline Float3A __vectorcall GetRow(int i) const { assert(i >= 0 && i < 4); return _xm.r[i]; }
		inline void __vectorcall SetRow(int i, const Float3A& value) { assert(i >= 0 && i < 4); _xm.r[i] = value; }

		PROPERTY_INDEXABLE(Float4A, Column);
		inline Float4A GetColumn(int j) const {
			assert(j >= 0 && j < 3); return Float4A(
				XMVectorGetByIndex(_xm.r[0], j),
				XMVectorGetByIndex(_xm.r[1], j),
				XMVectorGetByIndex(_xm.r[2], j),
				XMVectorGetByIndex(_xm.r[3], j));
		}
		inline void SetColumn(int j, const Float4& value)
		{
			assert(j >= 0 && j < 3);
			XMVectorSetByIndex(_xm.r[0], value.X, j);
			XMVectorSetByIndex(_xm.r[1], value.Y, j);
			XMVectorSetByIndex(_xm.r[2], value.Z, j);
			XMVectorSetByIndex(_xm.r[3], value.W, j);
		}

		inline Float4x3A& operator =(const Float4x3& rhs);
		inline Float4x3A& operator =(const XMMATRIX& xm) { _xm = xm; return *this; }
		inline Float4x3A& __vectorcall operator =(const Float4x3A& rhs) { _xm = rhs._xm; return *this; }
		inline Float4x3A __vectorcall operator +(const Float4x3A& rhs) const { return _xm + rhs._xm; }
		inline Float4x3A __vectorcall operator *(const Float4x3A& rhs) const { return _xm * rhs._xm; }
		inline Float4x4A __vectorcall operator *(const Float4x4A& rhs) const { return _xm * rhs; }
		inline Float4x3A __vectorcall operator *(const Float4x3& rhs) const;
		inline Float4x4A __vectorcall operator *(const Float4x4& rhs) const { return _xm * (XMMATRIX)rhs; }
		inline Float4x3A __vectorcall operator -(const Float4x3A& rhs) const { return _xm - rhs._xm; }
		inline Float4x3A __vectorcall operator *(float scalar) const { return _xm * scalar; }
		inline Float4x3A __vectorcall operator /(float scalar) const { return _xm / scalar; }
		inline Float4x3A& __vectorcall operator +=(const Float4x3A& rhs) { _xm = _xm + rhs; return *this; }
		inline Float4x3A& __vectorcall operator -=(const Float4x3A& rhs) { _xm = _xm - rhs; return *this; }
		inline Float4x3A& __vectorcall operator *=(const Float4x3A& rhs) { _xm = _xm * rhs; return *this; }
		inline Float4x3A& __vectorcall operator *=(float scalar) { _xm = _xm * scalar; return *this; }
		inline Float4x3A& __vectorcall operator /=(float scalar) { _xm = _xm / scalar; return *this; }
		inline bool __vectorcall operator ==(const Float4x3A& r) const throw()
		{
			return XMVector4Equal(_xm.r[0], r._xm.r[0]) &&
				XMVector4Equal(_xm.r[0], r._xm.r[0]) &&
				XMVector4Equal(_xm.r[0], r._xm.r[0]) &&
				XMVector4Equal(_xm.r[0], r._xm.r[0]);
		}
		inline bool __vectorcall operator !=(const Float4x3A& r) const throw()
		{
			return XMVector4NotEqual(_xm.r[0], r._xm.r[0]) ||
				XMVector4NotEqual(_xm.r[0], r._xm.r[0]) ||
				XMVector4NotEqual(_xm.r[0], r._xm.r[0]) ||
				XMVector4NotEqual(_xm.r[0], r._xm.r[0]);
		}

		PROPERTY_READONLY(bool, IsNan);
		inline bool __vectorcall GetIsNan() const { return XMMatrixIsNaN(*this); }

		PROPERTY_READONLY(bool, IsInfinite);
		inline bool __vectorcall GetIsInfinite() const { return XMMatrixIsInfinite(*this); }

		PROPERTY_READONLY(bool, IsIdentity);
		inline bool __vectorcall GetIsIdentity() const { return XMMatrixIsIdentity(*this); }

		PROPERTY_READONLY(Float4x3A, Inverse);
		inline Float4x3A __vectorcall GetInverse() const { return XMMatrixInverse(nullptr, *this); }

		inline Float4x3A __vectorcall InverseWithHint(const Float4A& determinantSplat) { return XMMatrixInverse(&(XMVECTOR)determinantSplat, *this); }

		PROPERTY_READONLY(Float4x4A, Transpose);
		inline Float4x4A __vectorcall GetTranspose() const { return XMMatrixTranspose(*this); }

		PROPERTY_READONLY(float, Determinant);
		inline float __vectorcall GetDeterminant() const { return XMVectorGetX(XMMatrixDeterminant(*this)); }

		inline Float4A __vectorcall SplatDeterminant() const { return XMMatrixDeterminant(*this); }

		inline Float4x4A __vectorcall MultiplyThenTranspose(const Float4x4A& m) const { return XMMatrixMultiplyTranspose(*this, m); }
		inline Float4x4A __vectorcall MultiplyThenTranspose(const Float4x3A& m) const { return XMMatrixMultiplyTranspose(*this, m); }

		bool Decompose(Float3* outPosition, Quaternion* outOrientation, Float3* outScale, Float4x3A* outResidual = nullptr) const;

		inline static Float4x3A __vectorcall Translation(float x, float y, float z) { return XMMatrixTranslation(x, y, z); }
		inline static Float4x3A __vectorcall Translation(const Float3A& v) { return XMMatrixTranslationFromVector(v); }
		inline static Float4x3A __vectorcall Scaling(float x, float y, float z) { return XMMatrixScaling(x, y, z); }
		inline static Float4x3A __vectorcall Scaling(const Float3A& v) { return XMMatrixScalingFromVector(v); }
		inline static Float4x3A __vectorcall RotationX(float angle) { return XMMatrixRotationX(angle); }
		inline static Float4x3A __vectorcall RotationY(float angle) { return XMMatrixRotationY(angle); }
		inline static Float4x3A __vectorcall RotationZ(float angle) { return XMMatrixRotationZ(angle); }
		inline static Float4x3A __vectorcall RotationPitchYawRoll(float pitch, float yaw, float roll) { return XMMatrixRotationRollPitchYaw(pitch, yaw, roll); }
		inline static Float4x3A __vectorcall RotationPitchYawRoll(const Float3A& v) { return XMMatrixRotationRollPitchYawFromVector(v); }
		inline static Float4x3A __vectorcall RotationNormalAxisAngle(const Float3A& normalAxis, float angle) { return XMMatrixRotationNormal(normalAxis, angle); }
		inline static Float4x3A __vectorcall RotationAxisAngle(const Float3A& axis, float angle) { return XMMatrixRotationAxis(axis, angle); }
		inline static Float4x3A __vectorcall RotationQuaternion(const QuaternionA& q) { return XMMatrixRotationQuaternion(q); }
		inline static Float4x3A __vectorcall Transform2D(const Float2A& scale, const Float2A& rotationCenter, float rotationAngle, const Float2A& position) { return XMMatrixAffineTransformation2D(scale, rotationCenter, rotationAngle, position); }
		inline static Float4x3A __vectorcall Transform(const Float3A& scale, const Float3A& rotationCenter, const QuaternionA& rotation, const Float3A& position) { return XMMatrixAffineTransformation(scale, rotationCenter, rotation, position); }
		inline static Float4x3A __vectorcall Reflection(const Float4A& reflectionPlane) { return XMMatrixReflect(reflectionPlane); }
		inline static Float4x3A __vectorcall Shadow(const Float3A& lightPosition, const Float4A& shadowPlane) { return XMMatrixShadow(shadowPlane, lightPosition); }
		inline static Float4x3A __vectorcall LookAt(const Float3A& eyePosition, const Float3A& focusPt, const Float3A& up) { return XMMatrixLookAtLH(eyePosition, focusPt, up); }
		inline static Float4x3A __vectorcall LookTo(const Float3A& eyePosition, const Float3A& eyeDirection, const Float3A& up) { return XMMatrixLookToLH(eyePosition, eyeDirection, up); }
	};

	struct Float4x3
	{
		typedef Float4x3A AlignedType;
		typedef Float4x3 UnalignedType;

		static const Float4x3 Identity;
		static const Float4x3 Zero;
		static const Float4x3 One;

		union
		{
			struct
			{
				float _11, _12, _13;
				float _21, _22, _23;
				float _31, _32, _33;
				float _41, _42, _43;
			};
			float m[4][3];
		};

		Float4x3() { }
		Float4x3(
			float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22,
			float m30, float m31, float m32) :
			_11(m00), _12(m01), _13(m02),
			_21(m10), _22(m11), _23(m12),
			_31(m20), _32(m21), _33(m22),
			_41(m30), _42(m31), _43(m32) { }
		explicit Float4x3(const float* arr) :
			_11(arr[0]), _12(arr[1]), _13(arr[2]),
			_21(arr[3]), _22(arr[4]), _23(arr[5]),
			_31(arr[6]), _32(arr[7]), _33(arr[8]),
			_41(arr[9]), _42(arr[10]), _43(arr[11]) { }
		Float4x3(const Float4x3A& copy) { XMStoreFloat4x3((XMFLOAT4X3*)this, copy); }

		inline operator XMMATRIX() const { return XMLoadFloat4x3((XMFLOAT4X3*)this); }

		PROPERTY_INDEXABLE(Float3, Row);
		inline Float3 GetRow(int i) const { assert(i >= 0 && i < 4); return *(Float3*)m[i]; }
		inline void SetRow(int i, const Float3& value) { assert(i >= 0 && i < 4); *(Float3*)m[i] = value; }

		PROPERTY_INDEXABLE(Float4, Column);
		inline Float4 GetColumn(int j) const { assert(j >= 0 && j < 3); return Float4(m[0][j], m[1][j], m[2][j], m[3][j]); }
		inline void SetColumn(int j, const Float4& value) { assert(j >= 0 && j < 3); m[0][j] = value.X; m[1][j] = value.Y; m[2][j] = value.Z; m[3][j] = value.W; }

		inline Float4x3& operator =(const Float4x3& rhs) { memcpy((void*)m, (void*)rhs.m, sizeof(float) * 12); return *this; }
		inline Float4x3& __vectorcall operator =(const Float4x3A& rhs) { XMStoreFloat4x3((XMFLOAT4X3*)this, rhs); return *this; }
		inline Float4x3A __vectorcall operator +(const Float4x3A& rhs) const { return (XMMATRIX)*this + (XMMATRIX)rhs; }
		inline Float4x3A __vectorcall operator *(const Float4x3A& rhs) const { return (XMMATRIX)*this * (XMMATRIX)rhs; }
		inline Float4x4A __vectorcall operator *(const Float4x4A& rhs) const { return (XMMATRIX)*this * rhs; }
		inline Float4x3A __vectorcall operator -(const Float4x3A& rhs) const { return (XMMATRIX)*this - (XMMATRIX)rhs; }
		inline Float4x3A __vectorcall operator *(const Float4x3& rhs) const { return (XMMATRIX)*this * (XMMATRIX)rhs; }
		inline Float4x4A __vectorcall operator *(const Float4x4& rhs) const { return (XMMATRIX)*this * (XMMATRIX)rhs; }
		inline Float4x3A __vectorcall operator *(float scalar) const { return (XMMATRIX)*this * scalar; }
		inline Float4x3A __vectorcall operator /(float scalar) const { return (XMMATRIX)*this / scalar; }
		inline Float4x3& __vectorcall operator +=(const Float4x3A& rhs) { XMStoreFloat4x3((XMFLOAT4X3*)this, (XMMATRIX)*this + rhs); return *this; }
		inline Float4x3& __vectorcall operator -=(const Float4x3A& rhs) { XMStoreFloat4x3((XMFLOAT4X3*)this, (XMMATRIX)*this - rhs); return *this; }
		inline Float4x3& __vectorcall operator *=(const Float4x3A& rhs) { XMStoreFloat4x3((XMFLOAT4X3*)this, (XMMATRIX)*this * rhs); return *this; }
		inline Float4x3& __vectorcall operator *=(float scalar) { XMStoreFloat4x3((XMFLOAT4X3*)this, (XMMATRIX)*this * scalar); return *this; }
		inline Float4x3& __vectorcall operator /=(float scalar) { XMStoreFloat4x3((XMFLOAT4X3*)this, (XMMATRIX)*this / scalar); return *this; }
		bool operator ==(const Float4x3& r) const throw();
		bool operator !=(const Float4x3& r) const throw();

		PROPERTY_READONLY(bool, IsNan);
		inline bool __vectorcall GetIsNan() const { return XMMatrixIsNaN(*this); }

		PROPERTY_READONLY(bool, IsInfinite);
		inline bool __vectorcall GetIsInfinite() const { return XMMatrixIsInfinite(*this); }

		PROPERTY_READONLY(bool, IsIdentity);
		inline bool __vectorcall GetIsIdentity() const { return XMMatrixIsIdentity(*this); }

		PROPERTY_READONLY(Float4x3A, Inverse);
		inline Float4x3A __vectorcall GetInverse() const { return XMMatrixInverse(nullptr, *this); }

		inline Float4x3A __vectorcall InverseWithHint(const Float4A& determinantSplat) { return XMMatrixInverse(&(XMVECTOR)determinantSplat, *this); }

		PROPERTY_READONLY(Float4x4A, Transpose);
		inline Float4x4A __vectorcall GetTranspose() const { return XMMatrixTranspose(*this); }

		PROPERTY_READONLY(float, Determinant);
		inline float __vectorcall GetDeterminant() const { return XMVectorGetX(XMMatrixDeterminant(*this)); }

		inline Float4A __vectorcall SplatDeterminant() const { return XMMatrixDeterminant(*this); }

		inline Float4x4A __vectorcall MultiplyThenTranspose(const Float4x4A& m) const { return XMMatrixMultiplyTranspose(*this, m); }
		inline Float4x4A __vectorcall MultiplyThenTranspose(const Float4x3A& m) const { return XMMatrixMultiplyTranspose(*this, m); }

		bool Decompose(Float3* outPosition, Quaternion* outOrientation, Float3* outScale, Float4x3A* outResidual = nullptr) const;

		inline static Float4x3 __vectorcall Translation(float x, float y, float z) { return Float4x3(XMMatrixTranslation(x, y, z)); }
		inline static Float4x3 __vectorcall Translation(const Float3A& v) { return Float4x3(XMMatrixTranslationFromVector(v)); }
		inline static Float4x3 __vectorcall Scaling(float x, float y, float z) { return Float4x3(XMMatrixScaling(x, y, z)); }
		inline static Float4x3 __vectorcall Scaling(const Float3A& v) { return Float4x3(XMMatrixScalingFromVector(v)); }
		inline static Float4x3 __vectorcall RotationX(float angle) { return Float4x3(XMMatrixRotationX(angle)); }
		inline static Float4x3 __vectorcall RotationY(float angle) { return Float4x3(XMMatrixRotationY(angle)); }
		inline static Float4x3 __vectorcall RotationZ(float angle) { return Float4x3(XMMatrixRotationZ(angle)); }
		inline static Float4x3 __vectorcall RotationPitchYawRoll(float pitch, float yaw, float roll) { return Float4x3(XMMatrixRotationRollPitchYaw(pitch, yaw, roll)); }
		inline static Float4x3 __vectorcall RotationPitchYawRoll(const Float3A& v) { return Float4x3(XMMatrixRotationRollPitchYawFromVector(v)); }
		inline static Float4x3 __vectorcall RotationNormalAxisAngle(const Float3A& normalAxis, float angle) { return Float4x3(XMMatrixRotationNormal(normalAxis, angle)); }
		inline static Float4x3 __vectorcall RotationAxisAngle(const Float3A& axis, float angle) { return Float4x3(XMMatrixRotationAxis(axis, angle)); }
		inline static Float4x3 __vectorcall RotationQuaternion(const QuaternionA& q) { return Float4x3(XMMatrixRotationQuaternion(q)); }
		inline static Float4x3 __vectorcall Transform2D(const Float2A& scale, const Float2A& rotationCenter, float rotationAngle, const Float2A& position) { return Float4x3(XMMatrixAffineTransformation2D(scale, rotationCenter, rotationAngle, position)); }
		inline static Float4x3 __vectorcall Transform(const Float3A& scale, const Float3A& rotationCenter, const QuaternionA& rotation, const Float3A& position) { return Float4x3(XMMatrixAffineTransformation(scale, rotationCenter, rotation, position)); }
		inline static Float4x3 __vectorcall Reflection(const Float4A& reflectionPlane) { return Float4x3(XMMatrixReflect(reflectionPlane)); }
		inline static Float4x3 __vectorcall Shadow(const Float3A& lightPosition, const Float4A& shadowPlane) { return Float4x3(XMMatrixShadow(shadowPlane, lightPosition)); }
		inline static Float4x3 __vectorcall LookAt(const Float3A& eyePosition, const Float3A& focusPt, const Float3A& up) { return Float4x3(XMMatrixLookAtLH(eyePosition, focusPt, up)); }
		inline static Float4x3 __vectorcall LookTo(const Float3A& eyePosition, const Float3A& eyeDirection, const Float3A& up) { return Float4x3(XMMatrixLookToLH(eyePosition, eyeDirection, up)); }
	};

	inline Float4x3A __vectorcall operator *(float scalar, const Float4x4& m) { return (XMMATRIX)m * scalar; }
	inline Float4x3A __vectorcall operator *(float scalar, const Float4x4A& m) { return (XMMATRIX)m * scalar; }
	inline Float4x3A __vectorcall operator *(float scalar, const Float4x3& m) { return (XMMATRIX)m * scalar; }
	inline Float4x3A __vectorcall operator *(float scalar, const Float4x3A& m) { return (XMMATRIX)m * scalar; }

	struct Float3x3 : public DirectX::XMFLOAT3X3
	{
		Float3x3() {}
		Float3x3(float m00, float m01, float m02,
			float m10, float m11, float m12,
			float m20, float m21, float m22) : DirectX::XMFLOAT3X3(m00, m01, m02, m10, m11, m12, m20, m21, m22) { }
		Float3x3(const XMMATRIX& m) { XMStoreFloat3x3(this, m); }
		explicit Float3x3(const float* pArray) : DirectX::XMFLOAT3X3(pArray) { }

		//		inline operator XMMATRIX() const { return XMLoadFloat3x3(this); }
		inline operator Float4x4A() const { return XMLoadFloat3x3(this); }

		PROPERTY_INDEXABLE(Float3, Row);
		inline Float3 GetRow(int i) { assert(i >= 0 && i < 3); return *(Float3*)m[i]; }
		inline void SetRow(int i, const Float3& value) { assert(i >= 0 && i < 3); *(Float3*)m[i] = value; }

		PROPERTY_INDEXABLE(Float3, Column);
		inline Float3 GetColumn(int j) const { assert(j >= 0 && j < 3); return Float3(m[0][j], m[1][j], m[2][j]); }
		inline void SetColumn(int j, const Float3& value) { assert(j >= 0 && j < 3); m[0][j] = value.X; m[1][j] = value.Y; m[2][j] = value.Z; }

		Float3x3& operator= (const Float3x3& rhs) { DirectX::XMFLOAT3X3::operator=(rhs); return *this; }
		bool operator ==(const Float3x3& rhs) const throw();
		bool operator !=(const Float3x3& rhs) const throw();

		static const Float3x3 Identity;
		static const Float3x3 Zero;
		static const Float3x3 One;

		inline static Float3x3 __vectorcall Scaling(float x, float y, float z) { return XMMatrixScaling(x, y, z); }
		inline static Float3x3 __vectorcall Scaling(const Float3A& v) { return XMMatrixScalingFromVector(v); }
		inline static Float3x3 __vectorcall RotationX(float angle) { return XMMatrixRotationX(angle); }
		inline static Float3x3 __vectorcall RotationY(float angle) { return XMMatrixRotationY(angle); }
		inline static Float3x3 __vectorcall RotationZ(float angle) { return XMMatrixRotationZ(angle); }
		inline static Float3x3 __vectorcall RotationPitchYawRoll(float pitch, float yaw, float roll) { return XMMatrixRotationRollPitchYaw(pitch, yaw, roll); }
		inline static Float3x3 __vectorcall RotationPitchYawRoll(const Float3A& v) { return XMMatrixRotationRollPitchYawFromVector(v); }
		inline static Float3x3 __vectorcall RotationNormalAxisAngle(const Float3A& normalAxis, float angle) { return XMMatrixRotationNormal(normalAxis, angle); }
		inline static Float3x3 __vectorcall RotationAxisAngle(const Float3A& axis, float angle) { return XMMatrixRotationAxis(axis, angle); }
		inline static Float3x3 __vectorcall RotationQuaternion(const QuaternionA& q) { return XMMatrixRotationQuaternion(q); }
	};

	inline Float4x4A& Float4x4A::operator =(const Float4x4& rhs) { _xm = (XMMATRIX)rhs; return *this; }
	inline Float4x3A& Float4x3A::operator =(const Float4x3& rhs) { _xm = (XMMATRIX)rhs; return *this; }

	inline Float2A Float2A::Transform(const Float4x3A& m) const { return XMVector2Transform(*this, m); }
	inline Float2A Float2A::Transform(const Float4x4A& m) const { return XMVector2Transform(*this, m); }
	inline Float2A Float2A::TransformNormal(const Float4x3A& m) const { return XMVector2TransformNormal(*this, m); }
	inline Float2A Float2A::TransformNormal(const Float4x4A& m) const { return XMVector2TransformNormal(*this, m); }
	inline Float2A Float2A::Project(const Float4x3A& m) const { return XMVector2TransformCoord(*this, m); }
	inline Float2A Float2A::Project(const Float4x4A& m) const { return XMVector2TransformCoord(*this, m); }
	inline Float2A Float2A::operator *(const Float4x3A& m) const { return XMVector2Transform(*this, m); }
	inline Float2A Float2A::operator *(const Float4x4A& m) const { return XMVector2Transform(*this, m); }
	inline Float2A& Float2A::operator *=(const Float4x3A& m) { *this = (AlignedType)XMVector2Transform(*this, m); return *this; }
	inline Float2A& Float2A::operator *=(const Float4x4A& m) { *this = (AlignedType)XMVector2Transform(*this, m); return *this; }

	inline Float2A Float2::Transform(const Float4x3A& m) const { return XMVector2Transform(*this, m); }
	inline Float2A Float2::Transform(const Float4x4A& m) const { return XMVector2Transform(*this, m); }
	inline Float2A Float2::TransformNormal(const Float4x3A& m) const { return XMVector2TransformNormal(*this, m); }
	inline Float2A Float2::TransformNormal(const Float4x4A& m) const { return XMVector2TransformNormal(*this, m); }
	inline Float2A Float2::Project(const Float4x3A& m) const { return XMVector2TransformCoord(*this, m); }
	inline Float2A Float2::Project(const Float4x4A& m) const { return XMVector2TransformCoord(*this, m); }
	inline Float2A Float2::operator *(const Float4x3A& m) const { return XMVector2Transform(*this, m); }
	inline Float2A Float2::operator *(const Float4x4A& m) const { return XMVector2Transform(*this, m); }
	inline Float2& Float2::operator *=(const Float4x3A& m) { *this = (AlignedType)XMVector2Transform(*this, m); return *this; }
	inline Float2& Float2::operator *=(const Float4x4A& m) { *this = (AlignedType)XMVector2Transform(*this, m); return *this; }

	inline Float3A Float3A::Transform(const Float4x3A& m) const { return XMVector3Transform(*this, m); }
	//inline Float3A Float3A::Transform(const Float4x4A& m) const { return XMVector3Transform(*this, m); }
	inline Float3A Float3A::TransformNormal(const Float4x3A& m) const { return XMVector3TransformNormal(*this, m); }
	//inline Float3A Float3A::TransformNormal(const Float4x4A& m) const { return XMVector3TransformNormal(*this, m); }
	inline Float3A Float3A::Project(const Float4x4A& m) const { return XMVector3TransformCoord(*this, m); }
	inline Float3A Float3A::operator *(const Float4x3A& m) const { return XMVector3Transform(*this, m); }
	//inline Float3A Float3A::operator *(const Float4x4A& m) const { return XMVector3Transform(*this, m); }
	inline Float3A& Float3A::operator *=(const Float4x3A& m) { *this = (AlignedType)XMVector3Transform(*this, m); return *this; }
	//inline Float3A& Float3A::operator *=(const Float4x4A& m) { *this = (AlignedType)XMVector3Transform(*this, m); return *this; }

	inline Float3A Float3::Transform(const Float4x3A& m) const { return XMVector3Transform(*this, m); }
	//inline Float3A Float3::Transform(const Float4x4A& m) const { return XMVector3Transform(*this, m); }
	inline Float3A Float3::TransformNormal(const Float4x3A& m) const { return XMVector3TransformNormal(*this, m); }
	//inline Float3A Float3::TransformNormal(const Float4x4A& m) const { return XMVector3TransformNormal(*this, m); }
	inline Float3A Float3::Project(const Float4x4A& m) const { return XMVector3TransformCoord(*this, m); }
	inline Float3A Float3::operator *(const Float4x3A& m) const { return XMVector3Transform(*this, m); }
	//inline Float3A Float3::operator *(const Float4x4A& m) const { return XMVector3Transform(*this, m); }
	inline Float3& Float3::operator *=(const Float4x3A& m) { *this = (AlignedType)XMVector3Transform(*this, m); return *this; }
	//inline Float3& Float3::operator *=(const Float4x4A& m) { *this = (AlignedType)XMVector3Transform(*this, m); return *this; }

	//inline Float4A Float4A::Transform(const Float4x3A& m) const { return XMVector4Transform(*this, m); }
	inline Float4A Float4A::Transform(const Float4x4A& m) const { return XMVector4Transform(*this, m); }
	//inline Float4A Float4A::operator *(const Float4x3A& m) const { return XMVector4Transform(*this, m); }
	inline Float4A Float4A::operator *(const Float4x4A& m) const { return XMVector4Transform(*this, m); }
	//inline Float4A& Float4A::operator *=(const Float4x3A& m) { *this = (AlignedType)XMVector4Transform(*this, m); return *this; }
	inline Float4A& Float4A::operator *=(const Float4x4A& m) { *this = (AlignedType)XMVector4Transform(*this, m); return *this; }

	//inline Float4A Float4::Transform(const Float4x3A& m) const { return XMVector4Transform(*this, m); }
	inline Float4A Float4::Transform(const Float4x4A& m) const { return XMVector4Transform(*this, m); }
	//inline Float4A Float4::operator *(const Float4x3A& m) const { return XMVector4Transform(*this, m); }
	inline Float4A Float4::operator *(const Float4x4A& m) const { return XMVector4Transform(*this, m); }
	//inline Float4& Float4::operator *=(const Float4x3A& m) { *this = (AlignedType)XMVector4Transform(*this, m); return *this; }
	inline Float4& Float4::operator *=(const Float4x4A& m) { *this = (AlignedType)XMVector4Transform(*this, m); return *this; }

	inline Float4x3A __vectorcall Float4x4::operator *(const Float4x3A& rhs) const { return XMMatrixMultiply(*this, rhs); }
	inline Float4x3A __vectorcall Float4x4A::operator *(const Float4x3A& rhs) const { return XMMatrixMultiply(_xm, rhs); }
	inline Float4x4A __vectorcall Float4x4A::MultiplyThenTranspose(const Float4x3A& m) const { return XMMatrixMultiplyTranspose(*this, m); }
	inline Float4x4A __vectorcall Float4x4::MultiplyThenTranspose(const Float4x3A& m) const { return XMMatrixMultiplyTranspose(*this, m); }
	inline Float4x3A __vectorcall Float4x3A::operator *(const Float4x3& rhs) const { return _xm * (XMMATRIX)rhs; }
}

DECLARE_HASHABLE(::CS::Float2)
DECLARE_HASHABLE(::CS::Float3)
DECLARE_HASHABLE(::CS::Float4)
DECLARE_HASHABLE(::CS::Float2A)
DECLARE_HASHABLE(::CS::Float3A)
DECLARE_HASHABLE(::CS::Float4A)
DECLARE_HASHABLE(::CS::Int2)
DECLARE_HASHABLE(::CS::Int3)
DECLARE_HASHABLE(::CS::Int4)
DECLARE_HASHABLE(::CS::Int2A)
DECLARE_HASHABLE(::CS::Int3A)
DECLARE_HASHABLE(::CS::Int4A)
DECLARE_HASHABLE(::CS::Bool2)
DECLARE_HASHABLE(::CS::Bool3)
DECLARE_HASHABLE(::CS::Bool4)
DECLARE_HASHABLE(::CS::Bool2A)
DECLARE_HASHABLE(::CS::Bool3A)
DECLARE_HASHABLE(::CS::Bool4A)
DECLARE_HASHABLE(::CS::Quaternion)
DECLARE_HASHABLE(::CS::QuaternionA)
DECLARE_HASHABLE(::CS::Float3x3)
DECLARE_HASHABLE(::CS::Float4x3)
DECLARE_HASHABLE(::CS::Float4x4)
DECLARE_HASHABLE(::CS::Float4x3A)
DECLARE_HASHABLE(::CS::Float4x4A)

IS_VALUETYPE(::CS::Float2, "BAF5E9F7-D626-4B94-9436-52A1CAB75E32");
IS_VALUETYPE(::CS::Float3, "94014C8C-AEC8-492D-BBDF-64BF0CADEE36");
IS_VALUETYPE(::CS::Float4, "F6701F12-A5BF-4809-A1B1-33EF405C2A04");

IS_VALUETYPE(::CS::Float2A, "038E604C-626C-4CD3-AD30-E4FB33EB8795");
IS_VALUETYPE(::CS::Float3A, "1F857758-4897-4BE0-A9F4-D0F75CC95CF9");
IS_VALUETYPE(::CS::Float4A, "8B6DB859-ACB4-4F7A-94CB-11C0B6DF7CC2");

IS_VALUETYPE(::CS::Int2, "8C379C01-31F8-410B-BBDE-EF29CF010FA2");
IS_VALUETYPE(::CS::Int3, "A89E1D19-EC3C-45FE-876D-1AF67C5E7593");
IS_VALUETYPE(::CS::Int4, "6D822D82-2AAF-4D45-9260-8568C41439E0");

IS_VALUETYPE(::CS::Int2A, "74649EE4-FAE8-4285-AE20-730532B6AA79");
IS_VALUETYPE(::CS::Int3A, "52F8ADEF-CF2C-4840-974B-9DD7D9AC63ED");
IS_VALUETYPE(::CS::Int4A, "62840D4B-87D3-408B-B237-F4003AD7EEA6");

IS_VALUETYPE(::CS::Bool2, "E82177F0-264B-484A-BC7F-8CA2C158D0F7");
IS_VALUETYPE(::CS::Bool3, "E9296CD5-2634-45A9-B124-6B2657419D91");
IS_VALUETYPE(::CS::Bool4, "F516D8CF-696C-4C4D-BCEC-026E0B73AC8A");

IS_VALUETYPE(::CS::Bool2A, "8412C092-5178-498E-AA30-18A2A2CF0173");
IS_VALUETYPE(::CS::Bool3A, "4A09E825-BBC0-43F9-ACC6-47A9D05D563E");
IS_VALUETYPE(::CS::Bool4A, "3CBD364E-0E00-4063-B410-0A8AF48B2ACC");

IS_VALUETYPE(::CS::QuaternionA, "1FB75604-3293-4168-A0FA-26468E7DF2B7");
IS_VALUETYPE(::CS::Quaternion, "FEBDD969-AC96-4BF2-B8ED-106934A12140");

IS_VALUETYPE(::CS::Float4x4A, "18D58019-8EF6-4243-A12E-C1EFFBC82F80");
IS_VALUETYPE(::CS::Float4x3A, "490C1D72-8235-4415-8C9C-27EA73EBB606");
IS_VALUETYPE(::CS::Float4x4, "AC3C94A7-9698-4A1E-8F67-60CA5E56EFCC");
IS_VALUETYPE(::CS::Float4x3, "29C390B9-D5DB-468C-9A33-1C54051B49E2");
IS_VALUETYPE(::CS::Float3x3, "C9576106-C9CB-49EB-BA7C-F3D1B0F254C0");
