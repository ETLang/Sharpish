#include "Sharpish.h"

using namespace CS;

BoundingSphereA::BoundingSphereA(const BoundingSphere& copy) : _value(copy) { }

BoundingSphereA BoundingSphereA::OfBox(const BoundingBoxA& box)
{
	return BoundingSphereA(box.Center, (box.Maxima - box.Minima).Length / 2);
}

BoundingSphere BoundingSphere::OfBox(const BoundingBox& box)
{
	return BoundingSphere(box.Center, (box.Maxima - box.Minima).Length / 2);
}