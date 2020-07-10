#pragma once

namespace CS
{
	struct Size
	{
		float Width, Height;

		Size() : Width(0), Height(0) { }
		Size(float width, float height) : Width(width), Height(height) { }

		PROPERTY_READONLY(float, Area);
		float GetArea() const {  return Width * Height; }

		bool operator ==(const Size& other) const { return Width == other.Width && Height == other.Height; }
		bool operator < (const Size& rhs) const { if(Width == rhs.Width) return Height < rhs.Height; return Width < rhs.Width; }
	};

	struct Size3D
	{
		float Width, Height, Depth;

		Size3D() : Width(0), Height(0), Depth(0) { }
		Size3D(float width, float height, float depth) : Width(width), Height(height), Depth(depth) { }

		PROPERTY_READONLY(float, Volume);
		float GetVolume() const {  return Width * Height * Depth; }

		bool operator ==(const Size3D& other) const { return Width == other.Width && Height == other.Height && Depth == other.Depth; }
		bool operator < (const Size3D& rhs) const { if(Width != rhs.Width) return Width < rhs.Width; if(Height != rhs.Height) return Height < rhs.Height; return Depth < rhs.Depth; }
	};
}

DECLARE_HASHABLE(::CS::Size);
DECLARE_HASHABLE(::CS::Size3D);
IS_VALUETYPE(::CS::Size, "BC8D3CDA-606B-4D75-9B12-922C74D78B91");
IS_VALUETYPE(::CS::Size3D, "680674FA-DE60-479D-9454-F9D28E855291");
