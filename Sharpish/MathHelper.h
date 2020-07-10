#pragma once

namespace CS
{
	namespace Help
	{
		class Math
		{
			static const float _ScaleUnitToMeters[9];
			static const float _ScaleUnitFromMeters[9];

		public:
			// TODO Remove this. It's a member of the matrix classes
			static bool SRTDecomposition(const Float4x4A& m,
				Float4x4A *outResidual, 
				Float3 *outPosition,
				Quaternion *outOrientation, 
				Float3 *outScale);

			// TODO Move this to a member of the matrix classes
			static float MatrixNorm(const Float4x4A& mat);

			// TODO Move this to a member of the matrix classes
			static bool IsApproximatelyIdentity(const Float3x3& mat, float eps = 1e-4);
			static bool IsApproximatelyIdentity(const Float4x3& mat, float eps = 1e-4);
			static bool IsApproximatelyIdentity(const Float4x4& mat, float eps = 1e-4);
			static bool IsApproximatelyIdentity(const Float4x3A& mat, float eps = 1e-4);
			static bool IsApproximatelyIdentity(const Float4x4A& mat, float eps = 1e-4);

			// TODO remove. Already a member of Float4x4
			static Float4x4 ComputeProjection(double fov, double aspect, const Range& depthRange);

			// TODO Remove. Already a member of matrix classes
			static Float3x3 LookAt(const Float3& at, const Float3& up);
			static Float4x3 LookAt(const Float3& position, const Float3& at, const Float3& up);

			// Solve Ax^2 + Bx + C == 0 for x
			// Returns true if solved successfully, false if the roots are imaginary. Imaginary results are not processed.
			static inline bool QuadraticSolve(float A, float B, float C, float* outX1, float* outX2);

			static uint32_t GCD(uint32_t a, uint32_t b);

			static BoundingSphere GetFrustumBoundingSphere(const Float4x4A& frustum);
			static bool ProjectPixelToRay(int x, int y, int width, int height, const Float4x4A& projection, Float3A& outOrigin, Float3A& outDirection);
			static bool ProjectPixelToRay(float ssx, float ssy, const Float4x4A& projection, Float3A& outOrigin, Float3A& outDirection);

			//static inline bool __vectorcall CullLine(const Float4x4A& projectionTransposed, const Float3A& lineAnchor, const Float3A& lineDirection);
			//static inline bool __vectorcall CullThickLine(const Float4x4A& projectionTransposed, const Float3A& lineAnchor, const Float3A& lineDirection, float radius);
			//static inline bool __vectorcall CullRay(const Float4x4A& projectionTransposed, const Float3A& rayOrigin, const Float3A& rayDirection);
			//static inline bool __vectorcall CullThickRay(const Float4x4A& projectionTransposed, const Float3A& rayOrigin, const Float3A& rayDirection, float radius);
			//static inline bool __vectorcall CullLineSeg(const Float4x4A& projectionTransposed, const Float3A& pointA, const Float3A& pointB);
			//static inline bool __vectorcall CullCylinder(const Float4x4A& projectionTransposed, const Float3A& pointA, const Float3A& pointB, float radius);
			//static inline bool __vectorcall CullSphere(const Float4x4A& projectionTransposed, const BoundingSphere& sphere);
			//static inline bool __vectorcall CullBox(const Float4x4A& projectionTransposed, const Float3A& minima, const Float3A& maxima);
			//static inline bool __vectorcall CullCone(const Float4x4A& projectionTransposed, const Float3A& origin, const Float3A& direction, float slope);

			//static bool IntersectLinePlane(const Float4A& plane, const Float3A& lineAnchor, const Float3A& lineDirection, Float3A& outPoint);
			//static bool IntersectPlanePlane(const Float4A& planeA, const Float4A& planeB, Float3A& outLineAnchor, Float3A& outLineDirection);
			static bool IntersectLineSphere(const BoundingSphereA& sphere, const Float3A& lineAnchor, const Float3A& lineDirection, Float3A& outPointA, Float3A& outPointB);
			static bool IntersectThickLineSphere(const BoundingSphereA& sphere, const Float3A& lineAnchor, const Float3A& lineDirection, float lineThickness, Float3A& outPointA, Float3A& outPointB);
			static int IntersectRaySphere(const BoundingSphereA& sphere, const Float3A& rayOrigin, const Float3A& rayDirection, Float3A& outPointA, Float3A& outPointB);
			static int IntersectThickRaySphere(const BoundingSphereA& sphere, const Float3A& rayOrigin, const Float3A& rayDirection, float rayThickness, Float3A& outPointA, Float3A& outPointB);
			static int IntersectLineSegSphere(const BoundingSphereA& sphere, const Float3A& lineA, const Float3A& lineB, Float3A& outPointA, Float3A& outPointB);
			static int IntersectThickLineSegSphere(const BoundingSphereA& sphere, const Float3A& lineA, const Float3A& lineB, float lineThickness, Float3A& outPointA, Float3A& outPointB);
			static bool IntersectLineFrustum(const Float4x4A& projectionTransposed, const Float3A& lineAnchor, const Float3A& lineDirection, Float3A& outPointA, Float3A& outPointB);
			static bool IntersectRayFrustum(const Float4x4A& projectionTransposed, const Float3A& rayOrigin, const Float3A& rayDirection, Float3A& outStartPoint, Float3A& outEndPoint);
			static bool IntersectLineSegFrustum(const Float4x4A& projectionTransposed, const Float3A& lineStart, const Float3A& lineEnd, Float3A& outStartPoint, Float3A& outEndPoint);
			static bool IntersectRayBoxEnter(const Float3A& rayOrigin, const Float3A& rayDirection, const BoundingBoxA& box);
			static bool IntersectRayBoxEnter(const Float3A& rayOrigin, const Float3A& rayDirection, const Float3A& minima, const Float3A& maxima);
			static bool IntersectRayBoxLeave(const Float3A& rayOrigin, const Float3A& rayDirection, const BoundingBoxA& box);
			static bool IntersectRayBoxLeave(const Float3A& rayOrigin, const Float3A& rayDirection, const Float3A& minima, const Float3A& maxima);

			static bool IntersectThickLineFrustum(const Float4x4A& projectionTransposed, const Float3A& lineAnchor, const Float3A& lineDirection, float lineRadius, Float3A& outPointA, Float3A& outPointB);
			static bool IntersectThickRayFrustum(const Float4x4A& projectionTransposed, const Float3A& rayStart, const Float3A& rayDirection, float rayRadius, Float3A& outStartPoint, Float3A& outEndPoint);
			static bool IntersectCylinderFrustum(const Float4x4A& projectionTransposed, const Float3A& pointA, const Float3A& pointB, float radius, Float3A& outPointA, Float3A& outPointB);

		};
	}
}