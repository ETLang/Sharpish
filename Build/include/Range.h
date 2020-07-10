#pragma once

namespace CS
{
	struct Range
	{
		float Minimum, Maximum;

		Range() : Minimum(0), Maximum(0) { }
		Range(float min, float max) : Minimum(min), Maximum(max) { }

		PROPERTY_READONLY(float, Span);
		float GetSpan() const {  return Maximum - Minimum; }

		PROPERTY_READONLY(float, Mean);
		float GetMean() const { return (Maximum + Minimum) / 2.0f; }

		bool operator ==(const Range& other) const { return Minimum == other.Minimum && Maximum == other.Maximum; }
		bool operator < (const Range& rhs) const { if(Minimum == rhs.Minimum) return Maximum < rhs.Maximum; return Minimum < rhs.Minimum; }
	};
}

IS_VALUETYPE(::CS::Range, "34EB500D-A05B-4812-8DBE-56635CFAF264");
DECLARE_HASHABLE(::CS::Range)
