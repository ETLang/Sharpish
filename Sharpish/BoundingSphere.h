#pragma once

#pragma push_macro("max")
#ifdef max
#undef max
#endif

namespace CS
{
	struct BoundingBox;
	struct BoundingSphere;

	struct __declspec(align(16))
	BoundingSphereA
	{
		Float4A _value;

	public:
		void* operator new(size_t sz)
		{
			return _aligned_malloc(sz, 16);
		}

		void* operator new(size_t sz, BoundingSphereA* where){ return where; }
		void operator delete(void* ptr) { _aligned_free(ptr); }
		void operator delete(void* ptr, BoundingSphereA* where) { assert(false); _aligned_free(ptr); }
		
		BoundingSphereA() : _value(Float4A::Zero) { }
		BoundingSphereA(const BoundingSphere& copy);

		BoundingSphereA(const Float4A& value) : _value(value) { } 
		BoundingSphereA(const Float3& center, float radius) : _value(center.X, center.Y, center.Z, radius) { }

		PROPERTY_READONLY(Float3A, Center);
		const Float3A& GetCenter() const { return _value.XYZ; }

		PROPERTY_READONLY(float, Radius);
		float GetRadius() const { return _value.W; }

		operator const Float4A&() const { return _value; }
		static BoundingSphereA OfBox(const BoundingBoxA& box);
	};

	struct BoundingSphere
	{
		Float4 _value;

	public:

		BoundingSphere() : _value(Float4::Zero) { }

		BoundingSphere(const BoundingSphereA& copy) : _value(copy) { }
		BoundingSphere(const Float4& value) : _value(value) { }
		BoundingSphere(const Float3& center, float radius) : _value(center.X, center.Y, center.Z, radius) { }

		PROPERTY_READONLY(Float3, Center);
		const Float3& GetCenter() const { return _value.XYZ; }

		PROPERTY_READONLY(float, Radius);
		float GetRadius() const { return _value.W; }

		operator const Float4&() const { return _value; }
		static BoundingSphere OfBox(const BoundingBox& box);
	};
}

DECLARE_HASHABLE(::CS::BoundingSphereA)
DECLARE_HASHABLE(::CS::BoundingSphere)
IS_VALUETYPE(::CS::BoundingSphereA, "AB7271AE-E3A7-4E14-8655-175B4FFAFDD4")
IS_VALUETYPE(::CS::BoundingSphere, "AB7271AE-E3A7-4E14-8655-175B4FFAFDD4")

#pragma pop_macro("max")