#include "Sharpish.h"

// ::PUBLICLIB::

using namespace CS;

const Float2 Float2::Zero(0.0f);
const Float2 Float2::One = XMVectorSplatOne();
const Float2 Float2::Infinity = XMVectorSplatInfinity();
const Float2 Float2::QNaN = XMVectorSplatQNaN();
const Float2 Float2::Epsilon = XMVectorSplatEpsilon();
const Float2 Float2::SignMask = XMVectorSplatSignMask();

const Float2A Float2A::Zero(0.0f);
const Float2A Float2A::One = XMVectorSplatOne();
const Float2A Float2A::Infinity = XMVectorSplatInfinity();
const Float2A Float2A::QNaN = XMVectorSplatQNaN();
const Float2A Float2A::Epsilon = XMVectorSplatEpsilon();
const Float2A Float2A::SignMask = XMVectorSplatSignMask();

const Float3 Float3::Zero(0.0f);
const Float3 Float3::One = XMVectorSplatOne();
const Float3 Float3::Infinity = XMVectorSplatInfinity();
const Float3 Float3::QNaN = XMVectorSplatQNaN();
const Float3 Float3::Epsilon = XMVectorSplatEpsilon();
const Float3 Float3::SignMask = XMVectorSplatSignMask();

const Float3A Float3A::Zero(0.0f);
const Float3A Float3A::One = XMVectorSplatOne();
const Float3A Float3A::Infinity = XMVectorSplatInfinity();
const Float3A Float3A::QNaN = XMVectorSplatQNaN();
const Float3A Float3A::Epsilon = XMVectorSplatEpsilon();
const Float3A Float3A::SignMask = XMVectorSplatSignMask();

const Float4 Float4::Zero(0.0f);
const Float4 Float4::One = XMVectorSplatOne();
const Float4 Float4::Infinity = XMVectorSplatInfinity();
const Float4 Float4::QNaN = XMVectorSplatQNaN();
const Float4 Float4::Epsilon = XMVectorSplatEpsilon();
const Float4 Float4::SignMask = XMVectorSplatSignMask();

const Float4A Float4A::Zero(0.0f);
const Float4A Float4A::One = XMVectorSplatOne();
const Float4A Float4A::Infinity = XMVectorSplatInfinity();
const Float4A Float4A::QNaN = XMVectorSplatQNaN();
const Float4A Float4A::Epsilon = XMVectorSplatEpsilon();
const Float4A Float4A::SignMask = XMVectorSplatSignMask();

const Int2 Int2::Zero(0);
const Int2 Int2::One(1);
const Int2 Int2::NegativeOne(-1);

const Int2A Int2A::Zero(0);
const Int2A Int2A::One(1);
const Int2A Int2A::NegativeOne(-1);

const Int3 Int3::Zero(0);
const Int3 Int3::One(1);
const Int3 Int3::NegativeOne(-1);

const Int3A Int3A::Zero(0);
const Int3A Int3A::One(1);
const Int3A Int3A::NegativeOne(-1);

const Int4 Int4::Zero(0);
const Int4 Int4::One(1);
const Int4 Int4::NegativeOne(-1);

const Int4A Int4A::Zero(0);
const Int4A Int4A::One(1);
const Int4A Int4A::NegativeOne(-1);

const Bool2 Bool2::True(true);
const Bool2 Bool2::False(false);

const Bool2A Bool2A::True(true);
const Bool2A Bool2A::False(false);

const Bool3 Bool3::True(true);
const Bool3 Bool3::False(false);

const Bool3A Bool3A::True(true);
const Bool3A Bool3A::False(false);

const Bool4 Bool4::True(true);
const Bool4 Bool4::False(false);

const Bool4A Bool4A::True(true);
const Bool4A Bool4A::False(false);

const QuaternionA QuaternionA::Zero(0, 0, 0, 0);
const QuaternionA QuaternionA::One(1, 1, 1, 1);
const QuaternionA QuaternionA::Identity = XMQuaternionIdentity();
const Quaternion Quaternion::Zero(0, 0, 0, 0);
const Quaternion Quaternion::One(1, 1, 1, 1);
const Quaternion Quaternion::Identity = QuaternionA::Identity;

const Float4x4A Float4x4A::Identity = XMMatrixIdentity();
const Float4x4A Float4x4A::Zero(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
const Float4x4A Float4x4A::One(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);

const Float4x4 Float4x4::Identity(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
const Float4x4 Float4x4::Zero(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
const Float4x4 Float4x4::One(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);

const Float4x3A Float4x3A::Identity = XMMatrixIdentity();
const Float4x3A Float4x3A::Zero(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
const Float4x3A Float4x3A::One(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);

const Float4x3 Float4x3::Identity(1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0);
const Float4x3 Float4x3::Zero(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
const Float4x3 Float4x3::One(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);

bool Float4x4A::Decompose(Float3* outPosition, Quaternion* outOrientation, Float3* outScale, Float4x4A* outResidual) const
{
	return Help::Math::SRTDecomposition(_xm, outResidual, outPosition, outOrientation, outScale);
}

bool Float4x4::Decompose(Float3* outPosition, Quaternion* outOrientation, Float3* outScale, Float4x4A* outResidual) const
{
	return Help::Math::SRTDecomposition(*this, outResidual, outPosition, outOrientation, outScale);
}

bool Float4x3A::Decompose(Float3* outPosition, Quaternion* outOrientation, Float3* outScale, Float4x3A* outResidual) const
{
	if (outResidual)
	{
		Float4x4A r;
		auto output = Help::Math::SRTDecomposition(_xm, &r, outPosition, outOrientation, outScale);
		*outResidual = r;
		return output;
	}
	else
		return Help::Math::SRTDecomposition(_xm, nullptr, outPosition, outOrientation, outScale);
}

bool Float4x3::Decompose(Float3* outPosition, Quaternion* outOrientation, Float3* outScale, Float4x3A* outResidual) const
{
	if (outResidual)
	{
		Float4x4A r;
		auto output = Help::Math::SRTDecomposition(*this, &r, outPosition, outOrientation, outScale);
		*outResidual = r;
		return output;
	}
	else
		return Help::Math::SRTDecomposition(*this, nullptr, outPosition, outOrientation, outScale);
}

Float4x4::Float4x4(const Float4x3A& copy)
{
	XMStoreFloat4x4((XMFLOAT4X4*)this, copy);
}

Float4x4::Float4x4(const Float4x3& copy) :
	_11(copy._11), _12(copy._12), _13(copy._13), _14(0),
	_21(copy._21), _22(copy._22), _23(copy._23), _24(0), 
	_31(copy._31), _32(copy._32), _33(copy._33), _34(0), 
	_41(copy._41), _42(copy._42), _43(copy._43), _44(1)
{

}

Float4x4A::Float4x4A(const Float4x3& copy) : _xm(XMLoadFloat4x3((XMFLOAT4X3*)&copy))
{

}

Float4x4A::Float4x4A(const Float4x3A& copy) : _xm((XMMATRIX)copy)
{

}

Bool2A::Vector(const Bool2& ua) : _xm(ua) { }
Bool3A::Vector(const Bool3& ua) : _xm(ua) { }
Bool4A::Vector(const Bool4& ua) : _xm(ua) { }

Float2A::Vector(const Float2& ua) : _xm(ua) { }
Float3A::Vector(const Float3& ua) : _xm(ua) { }
Float4A::Vector(const Float4& ua) : _xm(ua) { }

Int2A::Vector(const Int2& ua) : _xm(ua) { }
Int3A::Vector(const Int3& ua) : _xm(ua) { }
Int4A::Vector(const Int4& ua) : _xm(ua) { }

QuaternionA::QuaternionA(const Quaternion& ua) : _xm(ua) { }

Float4x4A::Float4x4A(const Float4x4& copy) : _xm(copy) { }
Float4x3A::Float4x3A(const Float4x3& copy) : _xm(copy) { }

const Float3x3 Float3x3::Identity = Float3x3(
	1, 0, 0, 
	0, 1, 0, 
	0, 0, 1 );

static const Float3x3 Zero(0, 0, 0, 0, 0, 0, 0, 0, 0);
static const Float3x3 One(1, 1, 1, 1, 1, 1, 1, 1, 1);

bool Float3x3::operator ==(const Float3x3& r) const throw() { return memcmp(this, &r, sizeof(Float3x3)) == 0; }
bool Float4x3::operator ==(const Float4x3& r) const throw() { return memcmp(this, &r, sizeof(Float4x3)) == 0; }
bool Float4x4::operator ==(const Float4x4& r) const throw() { return memcmp(this, &r, sizeof(Float4x4)) == 0; }
bool Float3x3::operator !=(const Float3x3& r) const throw() { return memcmp(this, &r, sizeof(Float3x3)) != 0; }
bool Float4x3::operator !=(const Float4x3& r) const throw() { return memcmp(this, &r, sizeof(Float4x3)) != 0; }
bool Float4x4::operator !=(const Float4x4& r) const throw() { return memcmp(this, &r, sizeof(Float4x4)) != 0; }
