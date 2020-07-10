#include "Sharpish.h"
#include "MathHelper.h"

using namespace CS;
using namespace std;

const float Help::Math::_ScaleUnitFromMeters[9] =
{
	1.00000f, //Unknown
	1.00000f, //Meters,
	1000.00f, //Millimeters,
	100.000f, //Centimeters,
	0.00100f, //Kilometers,
	39.3701f, //Inches,
	3.28084f, //Feet,
	1.09361f, //Yards,
	6.21371e-4f //Miles
};

const float Help::Math::_ScaleUnitToMeters[9] =
{
	1.00000f, //Unknown
	1.00000f, //Meters,
	0.00100f, //Millimeters,
	0.01000f, //Centimeters,
	1000.00f, //Kilometers,
	0.02540f, //Inches,
	0.30480f, //Feet,
	0.91440f, //Yards,
	1609.34f  //Miles
};

float AngleAround(const Float3A& start, const Float3A& end, const Float3A& axis)
{
    // Assume axis is already normalized
    auto startPlanar = start - axis * start.Dot(axis);
    auto endPlanar = end - axis * end.Dot(axis);

	startPlanar = startPlanar.Normalize();
	endPlanar = endPlanar.Normalize();

    bool neg = axis.Dot(startPlanar.Cross(endPlanar)) < -1e-8;
    float angle = (neg ? -1 : 1) * acos(max(-1.0f, min(1.0f, startPlanar.Dot(endPlanar))));
    return angle;
}

bool Help::Math::SRTDecomposition(const Float4x4A& m,
	Float4x4A *outResidual, 
	Float3 *outPosition,
	Quaternion *outOrientation, 
	Float3 *outScale)
{
	Float4x4A residual = m;

	float det = m.Determinant;

	bool insideOut = det < 0;

	if(fabs(det) < 1e-8)
		return false;

	Float3A xaxis = m.Row[0].XYZ;
	Float3A yaxis = m.Row[1].XYZ;
	Float3A zaxis = m.Row[2].XYZ;

	if(insideOut)
		xaxis *= -1;

	xaxis = xaxis.Normalize();
	yaxis = yaxis.Normalize();
	zaxis = zaxis.Normalize();

	if (outPosition)
		*outPosition = m.Row[3].XYZ;

	residual.Row[3] = Float4A(0, 0, 0, 1);

	Float3A endmid = (xaxis + yaxis + zaxis).Normalize();
	Float3A startmid = Float3A::One.Normalize();
	QuaternionA midrotation = QuaternionA::Identity;
    Float3A midRotAxis = startmid.Cross(endmid);

	if (midRotAxis.LengthSquared > 1e-8)
    {
        float angle = acos(max(-1.0f, min(1.0f, startmid.Dot(endmid))));
		midRotAxis = midRotAxis.Normalize();
		midrotation = QuaternionA::FromNormalAxisAngle(midRotAxis, angle);
	}

	Float4x4A midRotMat = Float4x4A::RotationQuaternion(midrotation);
		
    float xRot = AngleAround(midRotMat.Row[0].XYZ, xaxis, endmid);
    float yRot = AngleAround(midRotMat.Row[1].XYZ, yaxis, endmid);
    float zRot = AngleAround(midRotMat.Row[2].XYZ, zaxis, endmid);

	while(yRot - xRot > XM_PI) yRot -= XM_2PI;
	while(xRot - yRot > XM_PI) yRot += XM_2PI;

	while(zRot - xRot > XM_PI) zRot -= XM_2PI;
	while(xRot - zRot > XM_PI) zRot += XM_2PI;

	QuaternionA orient = midrotation * QuaternionA::FromAxisAngle(endmid, (xRot + yRot + zRot) / 3.0f);

	Float4x4A invRot = Float4x4A::RotationQuaternion(orient.Inverse);
		
	residual *= invRot;

	if (outOrientation)
		*outOrientation = orient;

	if(outScale)
	{
		outScale->X = residual.Row[0].XYZ.Length;
		outScale->Y = residual.Row[1].XYZ.Length;
		outScale->Z = residual.Row[2].XYZ.Length;

		if(insideOut) outScale->X *= -1;

		residual *= Float4x4A::Scaling(1.0f / *outScale);
	}

	if(outResidual)
		*outResidual = residual;

	return true;
}

float Help::Math::MatrixNorm(const Float4x4A& mat)
{
	float max = 0;

	for(int i = 0;i < 4;i++)
	{
		Float4A row = mat.Row[i];
		float rowsum = row.Abs().Dot(Float4A::One);
		max = MAX(max, rowsum);
	}

	return max;
}

bool Help::Math::IsApproximatelyIdentity(const Float3x3& mat, float eps)
{
	const float* x = &mat.m[0][0];
	const float* y = &Float3x3::Identity.m[0][0];
	const float* end = x + 9;
	while(x != end)
	{
		if(fabs(*x - *y) > eps) return false;

		x++; y++;
	}

	return true;
}

bool Help::Math::IsApproximatelyIdentity(const Float4x3& mat, float eps)
{
	const float* x = &mat.m[0][0];
	const float* y = &Float4x3::Identity.m[0][0];
	const float* end = x + 12;
	while(x != end)
	{
		if(fabs(*x - *y) > eps) return false;

		x++; y++;
	}

	return true;
}

bool Help::Math::IsApproximatelyIdentity(const Float4x4& mat, float eps)
{
	const float* x = &mat.m[0][0];
	const float* y = &Float4x4::Identity.m[0][0];
	const float* end = x + 16;
	while(x != end)
	{
		if(fabs(*x - *y) > eps) return false;

		x++; y++;
	}

	return true;
}

bool Help::Math::IsApproximatelyIdentity(const Float4x4A& mat, float eps)
{
	return IsApproximatelyIdentity(*(Float4x4*)&mat, eps);
}

bool Help::Math::IsApproximatelyIdentity(const Float4x3A& mat, float eps)
{
	return IsApproximatelyIdentity(*(Float4x4*)&mat, eps);
}

Float4x4 Help::Math::ComputeProjection(double fov, double aspect, const Range& depthRange)
{
	// Equivalent to D3DXMatrixPerspectiveFovLH
	double yscale = 1 / tan(fov * XM_PI / 360.0);
	double xscale = yscale / aspect;

	return Float4x4(
		(float)xscale, 0, 0, 0,
		0, (float)yscale, 0, 0,
		0, 0, depthRange.Maximum / depthRange.Span, 1,
		0, 0, -depthRange.Minimum * depthRange.Maximum / depthRange.Span, 0);
}

Float3x3 LookAt(const Float3& at, const Float3& up)
{
    Float3 z = at.Normalize();  // Forward
    Float3 x = up.Cross(z).Normalize(); // Right
    Float3 y = z.Cross(x);

    return Float3x3(
		x.X,  y.X,  z.X,
		x.Y,  y.Y,  z.Y,
        x.Z,  y.Z,  z.Z); 
}

Float4x3 Help::Math::LookAt(const Float3& position, const Float3& at, const Float3& up)
{
    Float3 z = (at - position).Normalize();  // Forward
    Float3 x = up.Cross(z).Normalize(); // Right
    Float3 y = z.Cross(x);

	return Float4x3(
		x.X, y.X, z.X,
		x.Y, y.Y, z.Y,
		x.Z, y.Z, z.Z,
		-x.Dot(position),
		-y.Dot(position),
		-z.Dot(position)); 
}

bool Help::Math::QuadraticSolve(float A, float B, float C, float* outX1, float* outX2)
{
	float rootExp = B*B-4*A*C;
	A *= 2;
	B *= -1;

	if(rootExp < 0)
		return false;

	float root = sqrt(rootExp);

	*outX1 = (B + root) / A;
	*outX2 = (B - root) / A;

	return true;
}

uint32_t Help::Math::GCD(uint32_t a, uint32_t b)
{
	while (a != b)
	{
		if (a < b)
			b = b - a;
		else
			a = a - b;
	}

	return a;
}

BoundingSphere Help::Math::GetFrustumBoundingSphere(const Float4x4A& frustum)
{
	static Float3A CornerA(1,1,0);
	static Float3A CornerB(1,1,1);
	static Float3A Near(0,0,0);
	static Float3A Far(0,0,1);

	auto inv = frustum.Inverse;

	auto cornerA = CornerA.Project(inv);
	auto cornerB = CornerB.Project(inv);
	auto n = Near.Project(inv);
	auto f = Far.Project(inv);

	auto disp = f - n;

	auto nA = n - cornerA;
	auto nB = n - cornerB;

	// |C-A| == |C-B|
	//|N+kVc-A|==|N+kVc-B|
	//(N-A+kVc).(N-A+kVc)==(N-B+kVc).(N-B+kVc)
	//Let An = N - A
	//Let Bn = N - B
	//(An+kVc).(An+kVc)==r^2
	//(Bn+kVc).(Bn+kVc)==r^2
	//An.An + 2k(An.Vc) + k^2(Vc.Vc) == Bn.Bn + 2k(Bn.Vc) + k^2(Vc.Vc)
	//(An.An-Bn.Bn) + 2k(An-Bn).Vc == 0
	//2k(An-Bn).Vc == Bn.Bn-An.An
	//k = (Bn.Bn-An.An)/(2(Ab-Bn).Vc)
	
	float k = (nB.LengthSquared - nA.LengthSquared) / (2 * (nA - nB).Dot(disp));

	auto center = n + disp * k;

	float radius = (cornerA - center).Length;
	return BoundingSphere(center, radius);
}

//bool Help::Math::CullLine(const Float4x4A& projectionTransposed, const Float3A& lineAnchor, const Float3A& lineDirection)
//{
//	return false;
//
//	/*
//				frustum planes:
//				Left: (column[3] + column[0])
//				Right: (column[3] - column[0])
//				Top: (column[3] - column[1])
//				Bottom: (column[3] + column[1])
//				Near: (column[2])
//				Far: (column[3] - column[2])
//
//				Finding points of intersection:
//				F: Frustum plane
//				F.p == 0
//
//				L: light line direction
//				Lo: Light origin
//				(Lo + L*k) == p
//
//				Equate and solve for k
//				F.(Lo + L*k) == 0
//				F.Lo + k*(F.L) == 0
//				k*(F.L) == -F.Lo
//				k = -F.Lo/(F.L)
//
//				Ranging:
//				kMin = float::min
//				kMax = float::max
//
//				if(L.Fnear !~= 0)
//				  kNear = Intersect(Fnear,L)
//				  kFar = Intersect(Ffar,L)
//				  if(kNear < kFar)
//				    kMin = kNear
//					kMax = kFar
//				  else
//				    kMin = kFar
//					kMax = kNear
//
//				dotLeft = FLeft.L
//				if(dotLeft << 0)
//				   k = -FLeft.Lo/dotLeft;
//				   if(
//				kLeft = Intersect(FLeft,L)
//				if(real(kkLeft > kMin)
//
//				if F.L ~= 0, skip
//				
//				
//				(0,0,1)*Vi
//				(0,0,-1
//				$eP - $sP = v
//
//				Ax+By+Cz+D=0
//
//				displacement units?
//
//	*/
//}

bool Help::Math::IntersectLineSphere(const BoundingSphereA& sphere, const Float3A& lineAnchor, const Float3A& lineDirection, Float3A& outPointA, Float3A& outPointB)
{
	Float4A spherev = sphere;

	Float3A ctol = lineAnchor - spherev;

	float A = lineDirection.LengthSquared;
	float B = 2 * lineDirection.Dot(ctol);
    float C = ctol.LengthSquared - (spherev.W * spherev.W);

	float k1,k2;

	if(!QuadraticSolve(A,B,C,&k1,&k2))
		return false;

	outPointA = lineAnchor + lineDirection * k1;
	outPointB = lineAnchor + lineDirection * k2;

	return true;
}

bool Help::Math::IntersectThickLineSphere(const BoundingSphereA& sphere, const Float3A& lineAnchor, const Float3A& lineDirection, float lineThickness, Float3A& outPointA, Float3A& outPointB)
{
	return IntersectLineSphere((Float4A)sphere + Float4A(0,0,0,lineThickness), lineAnchor, lineDirection, outPointA, outPointB);
}

int Help::Math::IntersectRaySphere(const BoundingSphereA& sphere, const Float3A& rayOrigin, const Float3A& rayDirection, Float3A& outPointA, Float3A& outPointB)
{
    Float3A ctol =  rayOrigin - sphere.Center;

	float A = rayDirection.LengthSquared;
	float B = 2 * rayDirection.Dot(ctol);
    float C = ctol.LengthSquared - (sphere.Radius * sphere.Radius);

	float k1,k2;

	if(!QuadraticSolve(A,B,C,&k1,&k2))
		return 0;

	if(k2 < k1)
		swap(k1, k2);

	if(k2 < 0)
		return 0; 

	int ret;

	if(k1 < 0)
	{
		k1 = 0;
		ret = 1;
	}
	else
		ret = 2;

	outPointA = rayOrigin + rayDirection * k1;
	outPointB = rayOrigin + rayDirection * k2;

	return ret;
}

int Help::Math::IntersectThickRaySphere(const BoundingSphereA& sphere, const Float3A& rayOrigin, const Float3A& rayDirection, float rayThickness, Float3A& outPointA, Float3A& outPointB)
{
	return IntersectRaySphere((Float4A)sphere + Float4A(0,0,0,rayThickness), rayOrigin - rayDirection.Normalize() * rayThickness, rayDirection, outPointA, outPointB);
}

int Help::Math::IntersectLineSegSphere(const BoundingSphereA& sphere, const Float3A& lineA, const Float3A& lineB, Float3A& outPointA, Float3A& outPointB)
{
	Float4A spherev = sphere;

	Float4 ctol = lineA - spherev;
	auto lineDirection = lineB - lineA;
	auto r = spherev.W;

	ctol.W = -r;
	float A = lineDirection.LengthSquared;
	float B = 2 * lineDirection.Dot(ctol.XYZ);
	float C = ctol.XYZ.LengthSquared -  r*r;

	float k1,k2;

	if(!QuadraticSolve(A,B,C,&k1,&k2))
		return 0;

	if(k2 < k1)
		swap(k1, k2);

	if(k2 < 0)
		return 0;

	if(k1 > 1)
		return 0;

	int ret = 2;

	if(k2 > 1)
		k2 = 1;

	if(k1 < 0)
		k1 = 0;

	outPointA = lineA + lineDirection * k1;
	outPointB = lineA + lineDirection * k2;

	return 1;
}

int Help::Math::IntersectThickLineSegSphere(const BoundingSphereA& sphere, const Float3A& lineA, const Float3A& lineB, float lineThickness, Float3A& outPointA, Float3A& outPointB)
{
	auto lv = (lineB - lineA).Normalize() * lineThickness;
	return IntersectLineSegSphere((Float4A)sphere + Float4A(0,0,0,lineThickness), lineA - lv, lineB + lv, outPointA, outPointB);
}

bool Help::Math::IntersectLineFrustum(const Float4x4A& projectionTransposed, const Float3A& lineAnchor, const Float3A& lineDirection, Float3A& outPointA, Float3A& outPointB)
{
	return false;
}

bool Help::Math::IntersectRayFrustum(const Float4x4A& projectionTransposed, const Float3A& rayOrigin, const Float3A& rayDirection, Float3A& outStartPoint, Float3A& outEndPoint)
{
	return false;
}

bool Help::Math::IntersectLineSegFrustum(const Float4x4A& projectionTransposed, const Float3A& lineStart, const Float3A& lineEnd, Float3A& outStartPoint, Float3A& outEndPoint)
{
	return false;
}
			
bool Help::Math::IntersectThickLineFrustum(const Float4x4A& projectionTransposed, const Float3A& lineAnchor, const Float3A& lineDirection, float lineRadius, Float3A& outPointA, Float3A& outPointB)
{
	return false;
}

bool Help::Math::IntersectThickRayFrustum(const Float4x4A& projectionTransposed, const Float3A& rayStart, const Float3A& rayDirection, float rayRadius, Float3A& outStartPoint, Float3A& outEndPoint)
{
	return false;
}

bool Help::Math::IntersectCylinderFrustum(const Float4x4A& projectionTransposed, const Float3A& pointA, const Float3A& pointB, float radius, Float3A& outPointA, Float3A& outPointB)
{
	return false;
}

bool Help::Math::IntersectRayBoxEnter(const Float3A& rayOrigin, const Float3A& rayDirection, const BoundingBoxA& box)
{
	return IntersectRayBoxEnter(rayOrigin, rayDirection, box.Minima, box.Maxima);
}

// TODO optimize this
bool Help::Math::IntersectRayBoxEnter(const Float3A& rayOrigin, const Float3A& rayDirection, const Float3A& minima, const Float3A& maxima)
{
	Float3 fOrigin = rayOrigin;
	Float3 fDirection = rayDirection;
	Float3 fMin = minima;
	Float3 fMax = maxima;

	// identify the (potential) planes of entry
	float x = (fDirection.X < 0) ? fMax.X : fMin.X;
	float y = (fDirection.Y < 0) ? fMax.Y : fMin.Y;
	float z = (fDirection.Z < 0) ? fMax.Z : fMin.Z;

	// test the direction. if the ray is pointing the wrong way it's a quick result
	if ((x - fOrigin.X) * fDirection.X < 0) return false;
	if ((y - fOrigin.Y) * fDirection.Y < 0) return false;
	if ((z - fOrigin.Z) * fDirection.Z < 0) return false;

	// Find the plane that intersects within the box boundaries.
	// o+kd=x
	// k = (x - o)/d
	if (fabs(fDirection.X) > 1e-5f)
	{
		float k = (x - fOrigin.X) / fDirection.X;
		float iy = fOrigin.Y + k * fDirection.Y;
		float iz = fOrigin.Z + k * fDirection.Z;

		if (iy <= fMax.Y && iy >= fMin.Y && iz <= fMax.Z && iz >= fMin.Z) return true;
	}

	if (fabs(fDirection.Y) > 1e-5f)
	{
		float k = (y - fOrigin.Y) / fDirection.Y;
		float ix = fOrigin.X + k * fDirection.X;
		float iz = fOrigin.Z + k * fDirection.Z;

		if (ix <= fMax.X && ix >= fMin.X && iz <= fMax.Z && iz >= fMin.Z) return true;
	}

	if (fabs(fDirection.Z) > 1e-5f)
	{
		float k = (z - fOrigin.Z) / fDirection.Z;
		float iy = fOrigin.Y + k * fDirection.Y;
		float ix = fOrigin.X + k * fDirection.X;

		if (iy <= fMax.Y && iy >= fMin.Y && ix <= fMax.X && ix >= fMin.X) return true;
	}

	return false;
}

bool Help::Math::IntersectRayBoxLeave(const Float3A& rayOrigin, const Float3A& rayDirection, const BoundingBoxA& box)
{
	return IntersectRayBoxLeave(rayOrigin, rayDirection, box.Minima, box.Maxima);
}

// TODO optimize this
bool Help::Math::IntersectRayBoxLeave(const Float3A& rayOrigin, const Float3A& rayDirection, const Float3A& minima, const Float3A& maxima)
{
	Float3 fOrigin = rayOrigin;
	Float3 fDirection = rayDirection;
	Float3 fMin = minima;
	Float3 fMax = maxima;

	// identify the planes of exit
	float x = (fDirection.X < 0) ? fMin.X : fMax.X;
	float y = (fDirection.Y < 0) ? fMin.Y : fMax.Y;
	float z = (fDirection.Z < 0) ? fMin.Z : fMax.Z;

	// test the direction. if the ray is pointing the wrong way it's a quick result
	if ((x - fOrigin.X) * fDirection.X < 0) return false;
	if ((y - fOrigin.Y) * fDirection.Y < 0) return false;
	if ((z - fOrigin.Z) * fDirection.Z < 0) return false;

	// Find the plane that intersects within the box boundaries.
	// o+kd=x
	// k = (x - o)/d
	if (fabs(fDirection.X) > 1e-5f)
	{
		float k = (x - fOrigin.X) / fDirection.X;
		float iy = fOrigin.Y + k * fDirection.Y;
		float iz = fOrigin.Z + k * fDirection.Z;

		if (iy <= fMax.Y && iy >= fMin.Y && iz <= fMax.Z && iz >= fMin.Z) return true;
	}

	if (fabs(fDirection.Y) > 1e-5f)
	{
		float k = (y - fOrigin.Y) / fDirection.Y;
		float ix = fOrigin.X + k * fDirection.X;
		float iz = fOrigin.Z + k * fDirection.Z;

		if (ix <= fMax.X && ix >= fMin.X && iz <= fMax.Z && iz >= fMin.Z) return true;
	}

	if (fabs(fDirection.Z) > 1e-5f)
	{
		float k = (z - fOrigin.Z) / fDirection.Z;
		float iy = fOrigin.Y + k * fDirection.Y;
		float ix = fOrigin.X + k * fDirection.X;

		if (iy <= fMax.Y && iy >= fMin.Y && ix <= fMax.X && ix >= fMin.X) return true;
	}

	return false;
}

bool Help::Math::ProjectPixelToRay(int x, int y, int width, int height, const Float4x4A& projection, Float3A& outOrigin, Float3A& outDirection)
{
	return ProjectPixelToRay(2.0f * (float)x / (float)width - 1, 1 - 2.0f * (float)y / (float)height, projection, outOrigin, outDirection);
}

bool Help::Math::ProjectPixelToRay(float ssx, float ssy, const Float4x4A& projection, Float3A& outOrigin, Float3A& outDirection)
{
	if (fabs(ssx) >= 1 || fabs(ssy) >= 1) return false;

	auto nearVec = Float3A(ssx, ssy, 0);
	auto farVec = Float3A(ssx, ssy, 1);

	auto pinv = projection.Inverse;

	auto rayOrigin = nearVec.Project(pinv);
	auto rayEnd = farVec.Project(pinv);

	outDirection = rayEnd - rayOrigin;
	outOrigin = rayOrigin;

	return true;
}
