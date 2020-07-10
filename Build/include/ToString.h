#pragma once

namespace CS
{
	extern std::string ToString(const bool& value);
	extern std::string ToString(const char& value);
	extern std::string ToString(const int16_t& value);
	extern std::string ToString(const int32_t& value);
	extern std::string ToString(const int64_t& value);
	extern std::string ToString(const uint16_t& value);
	extern std::string ToString(const uint32_t& value);
	extern std::string ToString(const uint8_t& value);
	extern std::string ToString(const uint64_t& value);
	extern std::string ToString(const float& value);
	extern std::string ToString(const double& value);
	extern std::string ToString(const char* value);
	extern std::string ToString(const wchar_t* value);
	extern std::string ToString(const std::string& value);
	extern std::string ToString(const std::wstring& value);
	extern std::string ToString(const Float2& value);
	extern std::string ToString(const Float3& value);
	extern std::string ToString(const Float4& value);
	extern std::string ToString(const Float2A& value);
	extern std::string ToString(const Float3A& value);
	extern std::string ToString(const Float4A& value);
	extern std::string ToString(const Int2& value);
	extern std::string ToString(const Int3& value);
	extern std::string ToString(const Int4& value);
	extern std::string ToString(const Int2A& value);
	extern std::string ToString(const Int3A& value);
	extern std::string ToString(const Int4A& value);
	extern std::string ToString(const Bool2& value);
	extern std::string ToString(const Bool3& value);
	extern std::string ToString(const Bool4& value);
	extern std::string ToString(const Bool2A& value);
	extern std::string ToString(const Bool3A& value);
	extern std::string ToString(const Bool4A& value);
	extern std::string ToString(const Quaternion& value);
	extern std::string ToString(const QuaternionA& value);
	extern std::string ToString(const Float3x3& value);
	extern std::string ToString(const Float4x3& value);
	extern std::string ToString(const Float4x4& value);
	extern std::string ToString(const Size& value);
	extern std::string ToString(const Size3D& value);
	extern std::string ToString(const Range& value);
	extern std::string ToString(const Rect& value);
	extern std::string ToString(const BoundingBox& value);
	extern std::string ToString(const BoundingBoxA& value);
	extern std::string ToString(const BoundingSphere& value);
    extern std::string ToString(const RFrame& value);
	extern std::string ToString(const IObject* value);
	extern std::string ToString(const com_ptr<IObject>& value);
	extern std::string ToString(const Object* value);
	extern std::string ToString(const com_ptr<Object>& value);
	extern std::string ToString(const IID& value);
	extern std::string ToString(const type_info& info);
} 