#pragma once
#include "MathTypes.h"

namespace CS
{
    class RFrame
    {
    public:
        Float3 Position;
        Quaternion Rotation;

        RFrame() : Position(Float3::Zero), Rotation(Quaternion::Identity) { }
        RFrame(const Float3& position, const Quaternion& rotation) : Position(position), Rotation(rotation) { }

        inline static const RFrame& Identity() { static RFrame identity; return identity; }

        inline RFrame Inverse() const
        {
            auto rni = Rotation.GetInverse();
            return RFrame((-Position).Rotate(rni), rni);
        }

        inline RFrame operator*(const RFrame& rhs) const
        {
            return RFrame(Position.Rotate(rhs.Rotation) + rhs.Position, rhs.Rotation * Rotation);
        }

        inline static RFrame Interpolate(const RFrame& a, const RFrame& b, float u)
        {
            return RFrame(a.Position + (b.Position - a.Position) * u,
                Quaternion::Slerp(a.Rotation, b.Rotation, u));
        }

        inline RFrame operator-(const RFrame& rhs) const
        {
            return RFrame(Position - rhs.Position,
                rhs.Rotation * Rotation.GetInverse());
        }

        inline RFrame operator+(const RFrame& rhs) const
        {
            return RFrame(Position + rhs.Position,
                rhs.Rotation * Rotation);
        }

        inline RFrame operator*(float scale) const
        {
            return RFrame(Position * scale,
                Quaternion::Slerp(Quaternion::Identity, Rotation, scale));
        }

        inline static RFrame Interpolate(const RFrame& a, const RFrame& aTangent, const RFrame& b, const RFrame& bTangent, float u)
        {
            auto w0 = 2 * u * u * u - 3 * u * u + 1;
            auto w1 = u * u * u - 2 * u * u + u;
            auto w2 = -2 * u * u * u + 2 * u * u;
            auto w3 = u * u * u - u * u;

            return a * w0 + aTangent * w1 + b * w2 + bTangent * w3;
        }

        PROPERTY(float, NormSq);
        inline float GetNormSq() const
        {
            return Position.LengthSquared * (1 + Float3(Rotation.X, Rotation.Y, Rotation.Z).LengthSquared);
        }

        PROPERTY(float, Norm);
        inline float GetNorm() const
        {
            return sqrtf(NormSq);
        }
    };

    extern Float3 operator*(Float3 v, RFrame frame);
}