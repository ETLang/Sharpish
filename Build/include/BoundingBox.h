#pragma once

namespace CS
{
	struct BoundingBoxA;
	 
	struct BoundingBox
	{
		Float3 Minima;
		Float3 Maxima;

		BoundingBox();
		BoundingBox(const Float3& min, const Float3& max);
		BoundingBox(const Float3A& min, const Float3A& max);
		BoundingBox(const Float3A* pts, int count);
		BoundingBox(const BoundingBox& copy) { Minima = copy.Minima; Maxima = copy.Maxima; }
		BoundingBox(const BoundingBoxA& copy);
		
		bool operator ==(const BoundingBox& rhs) const;
		bool operator !=(const BoundingBox& rhs) const;
		bool operator <(const BoundingBox& rhs) const;

		BoundingBoxA __vectorcall AddPoint(const Float3A& pt) const;
		BoundingBoxA AddPoints(Float3A* pts, int count) const;
		BoundingBoxA __vectorcall Combine(const BoundingBoxA &box) const;
		BoundingBoxA Transform(const Float4x3A &mat) const;

		bool IsIntersecting(const BoundingBox &box) const;

		void* operator new(size_t sz)
		{
			return _aligned_malloc(sz, 16);
		}

		PROPERTY_READONLY(Float3A, Center);
		Float3A GetCenter() const { return (Maxima + Minima) / 2; }

		PROPERTY_READONLY(Float3A, Size);
		Float3A GetSize() const { return Maxima - Minima; }

		PROPERTY_READONLY(float, Left);
		float GetLeft() const { return Minima.X; }

		PROPERTY_READONLY(float, Right);
		float GetRight() const { return Maxima.X; }

		PROPERTY_READONLY(float, Top);
		float GetTop() const { return Maxima.Y; }

		PROPERTY_READONLY(float, Bottom);
		float GetBottom() const { return Minima.Y; }

		PROPERTY_READONLY(float, Front);
		float GetFront() const { return Maxima.Z; }

		PROPERTY_READONLY(float, Back);
		float GetBack() const { return Minima.Z; }

		PROPERTY_READONLY(float, Width);
		float GetWidth() const { return Maxima.X - Minima.X; }

		PROPERTY_READONLY(float, Height);
		float GetHeight() const { return Maxima.Y - Minima.Y; }

		PROPERTY_READONLY(float, Depth);
		float GetDepth() const { return Maxima.Z - Minima.Z; }

		PROPERTY_READONLY(bool, Exists);
		bool GetExists() const { return Maxima.X >= Minima.X; }

	private:
		static Float3 MinVector;
		static Float3 MaxVector;

		inline void Apply(Float3* v);
	};

	struct BoundingBoxA
	{
		Float3A Minima;
		Float3A Maxima;

		BoundingBoxA();
		BoundingBoxA(const Float3A& min, const Float3A& max);
		BoundingBoxA(const Float3A* pts, int count);
		BoundingBoxA(const BoundingBox &copy) { Minima = copy.Minima; Maxima = copy.Maxima; }
		BoundingBoxA(const BoundingBoxA &copy) { Minima = copy.Minima; Maxima = copy.Maxima; }

		bool operator ==(const BoundingBoxA& rhs) const;
		bool operator !=(const BoundingBoxA& rhs) const;
		bool operator <(const BoundingBoxA& rhs) const;

		BoundingBoxA __vectorcall AddPoint(const Float3A& pt) const;
		BoundingBoxA AddPoints(Float3A* pts, int count) const;
		BoundingBoxA __vectorcall Combine(const BoundingBoxA &box) const;
		BoundingBoxA Transform(const Float4x3A &mat) const;

		bool IsIntersecting(const BoundingBoxA &box) const;

		void* operator new(size_t sz)
		{
			return _aligned_malloc(sz, 16);
		}

		void* operator new(size_t sz, BoundingBox* where){ return where; }
		void operator delete(void* ptr) { _aligned_free(ptr); }

		PROPERTY_READONLY(Float3A, Center);
		Float3A GetCenter() const { return (Maxima + Minima) / 2; }

		PROPERTY_READONLY(Float3A, Size);
		Float3A GetSize() const { return Maxima - Minima; }

		PROPERTY_READONLY(float, Left);
		float GetLeft() const{ return Minima.X; }

		PROPERTY_READONLY(float, Right);
		float GetRight() const { return Maxima.X; }

		PROPERTY_READONLY(float, Top);
		float GetTop() const { return Maxima.Y; }

		PROPERTY_READONLY(float, Bottom);
		float GetBottom() const { return Minima.Y; }

		PROPERTY_READONLY(float, Front);
		float GetFront() const { return Maxima.Z; }

		PROPERTY_READONLY(float, Back);
		float GetBack() const { return Minima.Z; }

		PROPERTY_READONLY(float, Width);
		float GetWidth() const { return Maxima.X - Minima.X; }

		PROPERTY_READONLY(float, Height);
		float GetHeight() const { return Maxima.Y - Minima.Y; }

		PROPERTY_READONLY(float, Depth);
		float GetDepth() const { return Maxima.Z - Minima.Z; }

		PROPERTY_READONLY(bool, Exists);
		bool GetExists() const { return Maxima.X >= Minima.X; }

	private:
		static Float3A MinVector;
		static Float3A MaxVector;

		inline void Apply(Float3* v);
	};
}

DECLARE_HASHABLE(::CS::BoundingBox)
DECLARE_HASHABLE(::CS::BoundingBoxA)
IS_VALUETYPE(::CS::BoundingBox, "9C1D7D3E-9042-42F6-AF18-905A98F862FB");
IS_VALUETYPE(::CS::BoundingBoxA, "179928A5-3F4D-41D5-959B-5D41C9A925D6");
