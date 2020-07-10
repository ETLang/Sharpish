#include "Sharpish.h"
#include <strsafe.h>
 
using namespace CS; 
using namespace std;

extern string CS::ToString(const bool& value)
{
	return value ? "true" : "false";
}

extern string CS::ToString(const char& value)
{
	return string(1, (char)value);
}

extern string CS::ToString(const int16_t& value)
{
	stringstream str;
	str << value;
	return str.str();
}

extern string CS::ToString(const int32_t& value)
{
	stringstream str;
	str << value;
	return str.str();
}

extern string CS::ToString(const int64_t& value)
{
	stringstream str;
	str << value;
	return str.str();
}

extern string CS::ToString(const uint16_t& value)
{
	stringstream str;
	str << value;
	return str.str();
}

extern string CS::ToString(const uint32_t& value)
{
	stringstream str;
	str << value;
	return str.str();
}

extern string CS::ToString(const uint8_t& value)
{
	return CS::ToString((uint32_t)value);
}

extern string CS::ToString(const uint64_t& value)
{
	stringstream str;
	str << value;
	return str.str();
}

extern string CS::ToString(const float& value)
{
	stringstream str;
	str << value;
	return str.str();
}

extern string CS::ToString(const double& value)
{
	stringstream str;
	str << value;
	return str.str();
}

extern string CS::ToString(const char* value)
{
	return value;
}

extern string CS::ToString(const wchar_t* value)
{
	return Help::String::WToS(value);
}

extern string CS::ToString(const string& value)
{
	return value;
}

extern string CS::ToString(const wstring& value)
{
	return Help::String::WToS(value);
}

extern string CS::ToString(const Float2& value)
{
	stringstream str;
	str << value.X << "," << value.Y;
	return str.str();
}

extern string CS::ToString(const Float3& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z;
	return str.str();
}

extern string CS::ToString(const Float4& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z << "," << value.W;
	return str.str();
}

extern string CS::ToString(const Float2A& value)
{
	stringstream str;
	str << value.X << "," << value.Y;
	return str.str();
}

extern string CS::ToString(const Float3A& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z;
	return str.str();
}

extern string CS::ToString(const Float4A& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z << "," << value.W;
	return str.str();
}

extern string CS::ToString(const Int2& value)
{
	stringstream str;
	str << value.X << "," << value.Y;
	return str.str();
}

extern string CS::ToString(const Int3& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z;
	return str.str();
}

extern string CS::ToString(const Int4& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z << "," << value.W;
	return str.str();
}

extern string CS::ToString(const Int2A& value)
{
	stringstream str;
	str << value.X << "," << value.Y;
	return str.str();
}

extern string CS::ToString(const Int3A& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z;
	return str.str();
}

extern string CS::ToString(const Int4A& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z << "," << value.W;
	return str.str();
}

extern string CS::ToString(const Bool2& value)
{
	stringstream str;
	str << value.X << "," << value.Y;
	return str.str();
}

extern string CS::ToString(const Bool3& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z;
	return str.str();
}

extern string CS::ToString(const Bool4& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z << "," << value.W;
	return str.str();
}

extern string CS::ToString(const Bool2A& value)
{
	stringstream str;
	str << value.X << "," << value.Y;
	return str.str();
}

extern string CS::ToString(const Bool3A& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z;
	return str.str();
}

extern string CS::ToString(const Bool4A& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z << "," << value.W;
	return str.str();
}

extern string CS::ToString(const Quaternion& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z << "," << value.W;
	return str.str();
}

extern string CS::ToString(const QuaternionA& value)
{
	stringstream str;
	str << value.X << "," << value.Y << "," << value.Z << "," << value.W;
	return str.str();
}

extern string CS::ToString(const Float3x3& value)
{
	stringstream str;
	str 
		<< value._11 << "," << value._12 << "," << value._13 << " / " 
		<< value._21 << "," << value._22 << "," << value._23 << " / " 
		<< value._31 << "," << value._32 << "," << value._33;
	return str.str();
}

extern string CS::ToString(const Float4x3& value)
{
	stringstream str;
	str
		<< value._11 << "," << value._12 << "," << value._13 << " / "
		<< value._21 << "," << value._22 << "," << value._23 << " / "
		<< value._31 << "," << value._32 << "," << value._33 << " / " 
		<< value._41 << "," << value._42 << "," << value._43;
	return str.str();
}

extern string CS::ToString(const Float4x4& value)
{
	stringstream str;
	str 
		<< value._11 << "," << value._12 << "," << value._13 << "," << value._14 << " / " 
		<< value._21 << "," << value._22 << "," << value._23 << "," << value._24 << " / " 
		<< value._31 << "," << value._32 << "," << value._33 << "," << value._34 << " / " 
		<< value._41 << "," << value._42 << "," << value._43 << "," << value._44;
	return str.str();
}

extern string CS::ToString(const Size& value)
{
	stringstream str;
	str << value.Width << "," << value.Height;
	return str.str();
}

extern string CS::ToString(const Size3D& value)
{
	stringstream str;
	str << value.Width << "," << value.Height << "," << value.Depth;
	return str.str();
}

extern string CS::ToString(const Range& value)
{
	stringstream str;
	if(value.Span == 0)
		str << value.Minimum;
	else
		str << "[" << value.Minimum << "," << value.Maximum << "]";
	return str.str();
}

extern string CS::ToString(const Rect& value)
{
	stringstream str;
	str << "X:[" << value.Left << "," << value.Right << ") Y:[" << value.Top << "," << value.Bottom << ")";
	return str.str();
}

extern string CS::ToString(const BoundingBox& value)
{
	stringstream str;
	str << "X:[" << value.Left << "," << value.Right << ") Y:[" << value.Top << "," << value.Bottom << ") Z:[" << value.Front << "," << value.Back << ")";
	return str.str();
}

extern string CS::ToString(const BoundingBoxA& value)
{
	stringstream str;
	str << "X:[" << value.Left << "," << value.Right << ") Y:[" << value.Top << "," << value.Bottom << ") Z:[" << value.Front << "," << value.Back << ")";
	return str.str();
}

extern string CS::ToString(const BoundingSphere& value)
{
	stringstream str;
	str << "C:<" << value.Center.X << "," << value.Center.Y << "," << value.Center.Z << "> R:" << value.Radius;
	return str.str();
}

extern string CS::ToString(const RFrame& value)
{
    stringstream str;
    str << "T<" << value.Position.X << ", " << value.Position.Y << ", " << value.Position.Z << "> R<"
        << value.Rotation.X << ", " << value.Rotation.Y << ", " << value.Rotation.Z << ", " << value.Rotation.W << ">";
    return str.str();
}

extern string CS::ToString(const com_ptr<IObject>& value)
{
	return ToString(value.Get());
}

extern string CS::ToString(const IObject* value)
{
	com_ptr<IBlob> blob;
	value->ToString(blob.GetAddressOf());
	return Help::String::FromBlob(blob.Get());
}

extern string CS::ToString(const com_ptr<Object>& value)
{
	return value->ToString();
}

extern string CS::ToString(const IID& value)
{
	string result = "{00000000-0000-0000-0000-000000000000}";

	StringCbPrintfA(&result[0], sizeof(char) * (result.length() + 1),
		"{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		value.Data1, value.Data2, value.Data3,
		(int)value.Data4[0], (int)value.Data4[1],
		(int)value.Data4[2], (int)value.Data4[3],
		(int)value.Data4[4], (int)value.Data4[5],
		(int)value.Data4[6], (int)value.Data4[7]);

	return result;
	//LPOLESTR oleStr;

	//StringFromIID(value, &oleStr);

	//string output(oleStr);
	//CoTaskMemFree(oleStr);

	//return output;
}

extern string CS::ToString(const type_info& info)
{
	return info.name();
}