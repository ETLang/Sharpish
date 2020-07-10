#pragma once

namespace CS
{
	struct Rect
	{
		float X, Y, Width, Height;

		Rect() : X(0), Y(0), Width(0), Height(0) { }
		Rect(float x, float y, float width, float height) : X(x), Y(y), Width(width), Height(height) { }
		Rect(const XMFLOAT4& copy) { *(XMFLOAT4*)this = copy; }
		Rect(const Range& xrange, const Range& yrange) : X(xrange.Minimum), Y(yrange.Minimum), Width(xrange.Span), Height(yrange.Span) { }
			
		PROPERTY_READONLY(float, Left);
		float GetLeft() const { return X; }

		PROPERTY_READONLY(float, Right);
		float GetRight() const { return X + Width; }
			
		PROPERTY_READONLY(float, Top);
		float GetTop() const { return Y; }

		PROPERTY_READONLY(float, Bottom);
		float GetBottom() const { return Y + Height; }

		operator const XMFLOAT4&() const { return *(XMFLOAT4*)this; }

		bool operator ==(const Rect& other) const 
			{ return X == other.X && Y == other.Y && Width == other.Width && Height == other.Height; }
		bool operator < (const Rect& rhs) const 
			{ if(X != rhs.X) return X < rhs.X; if(Y != rhs.Y) return Y < rhs.Y; if(Width != rhs.Width) return Width < rhs.Width; return Height < rhs.Height; }
	};
}

IS_VALUETYPE(::CS::Rect, "33FC5FE3-47E3-41D9-8367-B1A1A09C9D08");
DECLARE_HASHABLE(::CS::Rect)
