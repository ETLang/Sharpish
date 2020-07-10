#include "Sharpish.h"

// ::PUBLICLIB::

#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

#undef min
#undef max

#define MinFloat numeric_limits<float>::min()
#define MaxFloat numeric_limits<float>::max()

using namespace CS;
using namespace std;

Float3 BoundingBox::MaxVector(MaxFloat, MaxFloat, MaxFloat);
Float3 BoundingBox::MinVector(-MaxFloat, -MaxFloat, -MaxFloat);
Float3A BoundingBoxA::MaxVector(MaxFloat, MaxFloat, MaxFloat);
Float3A BoundingBoxA::MinVector(-MaxFloat, -MaxFloat, -MaxFloat);


BoundingBox::BoundingBox() :
	Minima(MaxVector), Maxima(MinVector)
{
}

BoundingBoxA::BoundingBoxA() :
	Minima(MaxVector), Maxima(MinVector)
{
}

BoundingBox::BoundingBox(const Float3& min, const Float3& max) :
	Minima(min), Maxima(max)
{
}

BoundingBox::BoundingBox(const BoundingBoxA& copy) : Minima(copy.Minima), Maxima(copy.Maxima)
{
}

BoundingBox::BoundingBox(const Float3A& min, const Float3A& max) :
	Minima(min), Maxima(max)
{
}

BoundingBoxA::BoundingBoxA(const Float3A& min, const Float3A& max) :
	Minima(min), Maxima(max)
{
}

BoundingBox::BoundingBox(const Float3A* pts, int count)
{
	if(count == 0)
	{
		Minima = MinVector;
		Maxima = MaxVector;
		return;
	}

	Minima = pts[0];
	Maxima = pts[0];

	for(int i = 1;i < count;i++)
	{
		Minima = Float3A::Min(Minima, pts[i]);
		Maxima = Float3A::Max(Maxima, pts[i]);
	}
}

BoundingBoxA::BoundingBoxA(const Float3A* pts, int count)
{
	if (count == 0)
	{
		Minima = MinVector;
		Maxima = MaxVector;
		return;
	}

	Minima = pts[0];
	Maxima = pts[0];

	for (int i = 1; i < count; i++)
	{
		Minima = Float3A::Min(Minima, pts[i]);
		Maxima = Float3A::Max(Maxima, pts[i]);
	}
}

void BoundingBox::Apply(Float3* v)
{
	*v = v->Clamp(Minima, Maxima);
}

void BoundingBoxA::Apply(Float3* v)
{
	*v = v->Clamp(Minima, Maxima);
}

BoundingBoxA BoundingBox::Combine(const BoundingBoxA &box) const
{
	return BoundingBoxA(Float3A::Min(Minima, box.Minima), Float3A::Max(Maxima, box.Maxima));
}

BoundingBoxA BoundingBoxA::Combine(const BoundingBoxA &box) const
{
	return BoundingBoxA(Float3A::Min(Minima, box.Minima), Float3A::Max(Maxima, box.Maxima));
}

BoundingBoxA BoundingBox::AddPoint(const Float3A& pt) const
{
	return BoundingBoxA(Float3A::Min(Minima, pt), Float3A::Max(Maxima, pt));
}

BoundingBoxA BoundingBoxA::AddPoint(const Float3A& pt) const
{
	return BoundingBoxA(Float3A::Min(Minima, pt), Float3A::Max(Maxima, pt));
}

BoundingBoxA BoundingBox::AddPoints(Float3A* pts, int count) const
{
	auto min = Minima;
	auto max = Maxima;

	for(int i = 0;i < count;i++)
	{
		min = Float3A::Min(min, pts[i]);
		max = Float3A::Max(max, pts[i]);
	}

	return BoundingBoxA(min, max);
}

BoundingBoxA BoundingBoxA::AddPoints(Float3A* pts, int count) const
{
	auto min = Minima;
	auto max = Maxima;

	for (int i = 0; i < count; i++)
	{
		min = Float3A::Min(min, pts[i]);
		max = Float3A::Max(max, pts[i]);
	}

	return BoundingBoxA(min, max);
}

BoundingBoxA BoundingBox::Transform(const Float4x3A &mat) const
{
	Float3A v[8];

	if(!Exists) return BoundingBoxA();

	auto L = Minima;
	auto H = Maxima;

	v[0] = Float3A(L.X, L.Y, L.Z) * mat;
	v[1] = Float3A(L.X, L.Y, H.Z) * mat;
	v[2] = Float3A(L.X, H.Y, L.Z) * mat;
	v[3] = Float3A(L.X, H.Y, H.Z) * mat;
	v[4] = Float3A(H.X, L.Y, L.Z) * mat;
	v[5] = Float3A(H.X, L.Y, H.Z) * mat;
	v[6] = Float3A(H.X, H.Y, L.Z) * mat;
	v[7] = Float3A(H.X, H.Y, H.Z) * mat;

	return BoundingBoxA(v, 8);
}

BoundingBoxA BoundingBoxA::Transform(const Float4x3A &mat) const
{
	Float3A v[8];

	if (!Exists) return BoundingBoxA();

	auto L = Minima;
	auto H = Maxima;

	v[0] = Float3A(L.X, L.Y, L.Z) * mat;
	v[1] = Float3A(L.X, L.Y, H.Z) * mat;
	v[2] = Float3A(L.X, H.Y, L.Z) * mat;
	v[3] = Float3A(L.X, H.Y, H.Z) * mat;
	v[4] = Float3A(H.X, L.Y, L.Z) * mat;
	v[5] = Float3A(H.X, L.Y, H.Z) * mat;
	v[6] = Float3A(H.X, H.Y, L.Z) * mat;
	v[7] = Float3A(H.X, H.Y, H.Z) * mat;

	return BoundingBoxA(v, 8);
}

bool BoundingBox::IsIntersecting(const BoundingBox &box) const
{
	return (Maxima.ComponentWise >= box.Maxima).All() && (Minima.ComponentWise <= box.Minima).All();
}

bool BoundingBoxA::IsIntersecting(const BoundingBoxA &box) const
{
	return (Maxima.ComponentWise >= box.Maxima).All() && (Minima.ComponentWise <= box.Minima).All();
}

bool BoundingBox::operator ==(const BoundingBox& rhs) const
{
	return Minima == rhs.Minima && Maxima == rhs.Maxima;
}

bool BoundingBoxA::operator ==(const BoundingBoxA& rhs) const
{
	return Minima == rhs.Minima && Maxima == rhs.Maxima;
}

bool BoundingBox::operator !=(const BoundingBox& rhs) const
{
	return Minima != rhs.Minima || Maxima != rhs.Maxima;
}

bool BoundingBoxA::operator !=(const BoundingBoxA& rhs) const
{
	return Minima != rhs.Minima || Maxima != rhs.Maxima;
}

bool BoundingBox::operator <(const BoundingBox& rhs) const
{
	if (Minima.X != rhs.Minima.X)
		return Minima.X < rhs.Minima.X;
	if (Minima.Y != rhs.Minima.Y)
		return Minima.Y < rhs.Minima.Y;
	if (Minima.Z != rhs.Minima.Z)
		return Minima.Z < rhs.Minima.Z;
	if (Maxima.X != rhs.Maxima.X)
		return Maxima.X < rhs.Maxima.X;
	if (Maxima.Y != rhs.Maxima.Y)
		return Maxima.Y < rhs.Maxima.Y;
	return Maxima.Z < rhs.Maxima.Z;
}

bool BoundingBoxA::operator <(const BoundingBoxA& rhs) const
{
	if (Minima.X != rhs.Minima.X)
		return Minima.X < rhs.Minima.X;
	if (Minima.Y != rhs.Minima.Y)
		return Minima.Y < rhs.Minima.Y;
	if (Minima.Z != rhs.Minima.Z)
		return Minima.Z < rhs.Minima.Z;
	if (Maxima.X != rhs.Maxima.X)
		return Maxima.X < rhs.Maxima.X;
	if (Maxima.Y != rhs.Maxima.Y)
		return Maxima.Y < rhs.Maxima.Y;
	return Maxima.Z < rhs.Maxima.Z;
}
