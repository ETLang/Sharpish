#include "Sharpish.h"

using namespace CS;
using namespace std;

const char* Help::String::_Whitespace = " \t\n\r\f\v";
const wchar_t* Help::String::_WWhitespace = L" \t\n\r\f\v";

wstring Help::String::SToW(const string& s)
{
	wstring temp(s.length(),L' ');
	copy(s.begin(), s.end(), temp.begin());
	return temp; 
}
 
string Help::String::WToS(const wstring& s)
{
	string temp(s.length(), ' ');
	auto out = temp.begin();
	for (auto c : s)
	{
		*out = (char)c;
		out++;
	}
	return temp; 
}

UUID Help::String::GenerateUUIDFromString(string str)
{
	union
	{
		uint16_t IdWords[8];
		UUID Uuid;
	} id;

	uint32_t idseed = 0;

	const char* seedStart = str.c_str();
	const char* seedEnd = seedStart + str.length();

	while(seedStart + 4 < seedEnd)
	{
		idseed ^= *(uint32_t*)seedStart;
		seedStart += 4;
	}

	while(seedStart < seedEnd)
	{
		idseed ^= (uint32_t)*(uint8_t*)seedStart;
		seedStart++;
	}
	
	auto oldSeed = rand();
	srand(idseed);

	for(int i = 0;i < 8;i++)
		id.IdWords[i] = (uint16_t)rand();


	srand(oldSeed);
	return id.Uuid;
}

UUID Help::String::GenerateUUIDFromString(wstring str)
{
	return GenerateUUIDFromString(WToS(str));
}

wstring Help::String::Format(wstring format, ...)
{
	wchar_t buffer[4096];

	buffer[0] = 0;

	va_list vargs;
	va_start(vargs,format);
	_vsnwprintf_s(buffer, 4096, format.c_str(), vargs);
	va_end(vargs);

	return buffer;
}

string Help::String::Format(string format, ...)
{
	char buffer[4096];

	buffer[0] = 0;

	va_list vargs;
	va_start(vargs,format);
	vsnprintf_s(buffer, 4096, format.c_str(), vargs);
	va_end(vargs);

	return buffer;
}

com_ptr<IBlob> Help::String::AsBlob(std::string str)
{
    size_t sz = str.length() + 1;

    auto blob = Make<Blob>(sz * sizeof(char));
    strcpy_s((char*)blob->GetData(), sz, str.c_str());
    return blob;
}

com_ptr<IBlob> Help::String::AsBlob(wstring str)
{
	size_t sz = str.length() + 1;

	auto blob = Make<Blob>(sz * sizeof(wchar_t));
	wcscpy_s((wchar_t*)blob->GetData(), sz, str.c_str());
	return blob;
}

wstring Help::String::D3DUnblob(IBlob* blob)
{
	if(!blob) 
		return L""; 
	
	string output((char*)blob->GetBufferPointer(), blob->GetBufferSize() / sizeof(char));
	return SToW(output); 
}

string Help::String::FromBlob(IBlob* blob)
{
	if(!blob)
		return "";

	return string((char*)blob->GetBufferPointer(), blob->GetBufferSize() / sizeof(char));
}
