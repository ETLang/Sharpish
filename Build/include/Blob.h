#pragma once

#include "Object.h"

namespace CS
{
	class comid("332B9C75-2B73-43A5-A656-14CEFE8385D0")
	Blob : public ComObject<Blob, Object, ID3DBlob>
	{
		void* _data;
		size_t _size;
		bool _aligned;
		bool _owned;

	public:
		Blob(byte* data, size_t size, bool aligned = false) : _data(data), _size(size), _aligned(aligned), _owned(true) { }
		Blob(size_t size) : _data(malloc(size)), _size(size), _aligned(0), _owned(true) { }
		~Blob() { if(!_owned) return; if(_aligned) _aligned_free(_data); else free(_data); }

		PROPERTY_READONLY(byte*, Data);
		byte* GetData() const { return (byte*)_data; }
        
		PROPERTY_READONLY(size_t, Size);
		size_t GetSize() const { return _size; }
        
		virtual void* __stdcall GetBufferPointer() { return _data; }
		virtual SIZE_T __stdcall GetBufferSize() { return _size; }

		void Disown() { _owned = false; }
	};
}