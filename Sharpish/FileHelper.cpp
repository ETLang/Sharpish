#include "Sharpish.h"

#include <fstream>

using namespace CS;
using namespace std;

vector<byte> Help::File::ReadWhole(wstring path)
{
	return ReadWhole(String::WToS(path));
}

vector<byte> Help::File::ReadWhole(string path)
{
	ifstream fs;
	fs.unsetf(ios::skipws);
	fs.open(path.c_str(), ios::binary | ios::in);

	if(!fs)
		throw Exception("File not found, or not readable");

	fs.seekg(0, ifstream::end);

	size_t length = (size_t)fs.tellg();
	fs.seekg(0, ifstream::beg);

	vector<byte> output(length); 
	fs.read((char*)&output[0], length);
	if(fs.fail())
		throw Exception("Unexpected failure reading file");

	return output; 
}

void Help::File::WriteWhole(wstring path, wstring contents)
{
	wofstream fs;
	fs.unsetf(ios::skipws);
	fs.open(path.c_str(), ios::out | ios::trunc);

	if(!fs)
		throw Exception("File not found, or not writable");

	fs << contents;
}

void Help::File::WriteWhole(string path, string contents)
{
	ofstream fs;
	fs.unsetf(ios::skipws);
	fs.open(path.c_str(), ios::out | ios::trunc);

	if(!fs)
		throw Exception("File not found, or not writable");

	fs << contents;
}

void Help::File::AppendTo(wstring path, wstring contents)
{
	wofstream fs;
	fs.unsetf(ios::skipws);
	fs.open(path.c_str(), ios::out | ios::app);

	if(!fs)
		throw Exception("File not found, or not writable");

	fs << contents;
}

void Help::File::AppendTo(string path, string contents)
{
	ofstream fs;
	fs.unsetf(ios::skipws);
	fs.open(path.c_str(), ios::out | ios::app);

	if(!fs)
		throw Exception("File not found, or not writable");

	fs << contents;
}

void Help::File::Delete(string path)
{
	DeleteFileA(path.c_str());
}

void Help::File::Delete(wstring path)
{
	DeleteFileW(path.c_str());
}

bool Help::File::FileExists(wstring path)
{
	FILE *file;
	_wfopen_s(&file, path.c_str(), L"r");

    if (file) {
        fclose(file);
        return true;
    } else {
        return false;
    }  
}

bool Help::File::DirectoryExists(const wstring& path)
{
	WIN32_FILE_ATTRIBUTE_DATA data;

	if (!GetFileAttributesEx(path.c_str(), GetFileExInfoStandard, &data))
		return false;

	if (data.dwFileAttributes == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	return 0 != (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

size_t Help::File::GetSize(wstring path)
{
	ifstream file(path, ios::binary);

	if(!file.is_open())
		return (size_t)-1;

	file.seekg(0, ios::end);
	return (size_t)file.tellg();
} 

size_t Help::File::GetSize(string path)
{
	return GetSize(String::SToW(path));
}

wstring Help::File::GetContainingDirectory(wstring path)
{
	size_t posb = path.rfind(L'\\', path.length() - 2);
	size_t posf = path.rfind(L'/', path.length() - 2);

	if(posf > posb && posf != wstring::npos)
		return path.substr(0, posf) + L'/';
	else if(posb != wstring::npos)
		return path.substr(0, posb) + L'\\';
	else if(path.size() > 2 && path[1] == L':')
		return L"";
	else
		return L".";
}

string Help::File::GetContainingDirectory(string path)
{
	size_t posb = path.rfind('\\', path.length() - 2);
	size_t posf = path.rfind('/', path.length() - 2);

	if(posf > posb && posf != string::npos)
		return path.substr(0, posf) + '/';
	else if(posb != string::npos)
		return path.substr(0, posb) + '\\';
	else if(path.size() > 2 && path[1] == ':')
		return "";
	else
		return ".";
}

wstring Help::File::GetName(wstring path)
{
	size_t len = path.length();
	size_t posb = path.rfind(L'\\');
	size_t posf = path.rfind(L'/');

	if(posf > posb && posf != wstring::npos)
	{
		if(*path.rbegin() == L'/')
			return path.substr(posf + 1, len - posf - 2);
		else
			return path.substr(posf + 1, len - posf - 1);
	}
	else if(posb != wstring::npos)
	{
		if(*path.rbegin() == L'\\')
			return path.substr(posb + 1, len - posb - 2);
		else
			return path.substr(posb + 1, len - posb - 1);
	}
	else if(*path.rbegin() == L'/' || *path.rbegin() == L'\\')
		return path.substr(0, len - 1);
	else
		return path;
}

string Help::File::GetName(string path)
{
	size_t len = path.length();
	size_t posb = path.rfind('\\');
	size_t posf = path.rfind('/');

	if(posf > posb && posf != string::npos)
	{
		if(*path.rbegin() == '/')
			return path.substr(posf + 1, len - posf - 2);
		else
			return path.substr(posf + 1, len - posf - 1);
	}
	else if(posb != string::npos)
	{
		if(*path.rbegin() == '\\')
			return path.substr(posb + 1, len - posb - 2);
		else
			return path.substr(posb + 1, len - posb - 1);
	}
	else if(*path.rbegin() == '/' || *path.rbegin() == '\\')
		return path.substr(0, len - 1);
	else
		return path;
}

wstring Help::File::GetNameNoExtension(wstring path)
{
	auto name = GetName(path);
	auto dot = name.rfind(L'.');

	if(dot != wstring::npos)
		return name.substr(0, dot);
	else
		return name;
}

string Help::File::GetNameNoExtension(string path)
{
	auto name = GetName(path);
	auto dot = name.rfind('.');

	if(dot != string::npos)
		return name.substr(0, dot);
	else
		return name;
}

wstring Help::File::GetExtension(wstring path)
{
	auto name = GetName(path);
	auto dot = name.rfind(L'.');

	if(dot != wstring::npos)
		return name.substr(dot + 1);
	else
		return name;
}

string Help::File::GetExtension(string path)
{
	auto name = GetName(path);
	auto dot = name.rfind('.');

	if(dot != string::npos)
		return name.substr(dot + 1);
	else
		return name;
}

wstring Help::File::CombinePath(wstring a, wstring b)
{
	if(a.empty()) return b;
	if(b.empty()) return a;

	if(*a.rbegin() == L'/' || *a.rbegin() == L'\\')
	{
		if(b[0] == L'/' || b[0] == L'\\')
		{
			if(b.size() == 1)
				return a;
			else
				return a + &b[1];
		}
		else
			return a + b;
	}
	else if(b[0] == L'/' || b[0] == L'\\')
	{
		return a + b;
	}
	else
	{
		auto abslash = a.find(L'\\');
		
		if(abslash != wstring::npos)
			return a + L'\\' + b;
		else
		{
			auto aslash = a.find(L'/');
			if(aslash != wstring::npos)
				return a + L'/' + b;
			else
			{
				auto bbslash = b.find(L'\\');

				if(bbslash != wstring::npos)
					return a + L'\\' + b;
				else
				{
					auto bslash = b.find(L'/');
					if(bslash != wstring::npos)
						return a + L'/' + b;
					else
						return a + L'\\' + b;
				}
			}
		}
	}
}

string Help::File::CombinePath(string a, string b)
{
	if(a.empty()) return b;
	if(b.empty()) return a;

	if(*a.rbegin() == '/' || *a.rbegin() == '\\')
	{
		if(b[0] == '/' || b[0] == '\\')
		{
			if(b.size() == 1)
				return a;
			else
				return a + &b[1];
		}
		else
			return a + b;
	}
	else if(b[0] == '/' || b[0] == '\\')
	{
		return a + b;
	}
	else
	{
		auto abslash = a.find('\\');
		
		if(abslash != string::npos)
			return a + '\\' + b;
		else
		{
			auto aslash = a.find('/');
			if(aslash != string::npos)
				return a + '/' + b;
			else
			{
				auto bbslash = b.find('\\');

				if(bbslash != string::npos)
					return a + '\\' + b;
				else
				{
					auto bslash = b.find('/');
					if(bslash != string::npos)
						return a + '/' + b;
					else
						return a + '\\' + b;
				}
			}
		}
	}
}

bool Help::File::IsRelativePath(wstring path)
{
	return !IsAbsolutePath(path);
}

bool Help::File::IsRelativePath(string path)
{
	return !IsAbsolutePath(path);
}

bool Help::File::IsAbsolutePath(wstring path)
{
	if(path.length() < 2) return false;
	return String::StartsWith(path, L"\\\\") || String::StartsWith(path, L"//") || path[1] == L':';
}

bool Help::File::IsAbsolutePath(string path)
{
	if(path.length() < 2) return false;
	return String::StartsWith(path, "\\\\") || String::StartsWith(path, "//") || path[1] == ':';
}

void Help::File::TokenizePath(wstring path, vector<wstring>& tokens)
{
	auto fixed = String::Replace(path, L"/", L"\\");		

	auto first = tokens.size();

	String::Split(fixed, L'\\', tokens);

	if(String::StartsWith(fixed, L"\\\\"))
		tokens[first] = L"\\\\" + tokens[first];
}

void Help::File::TokenizePath(string path, vector<string>& tokens)
{
	auto fixed = String::Replace(path, "/", "\\");		

	auto first = tokens.size();

	String::Split(fixed, '\\', tokens);

	if(String::StartsWith(fixed, "\\\\"))
		tokens[first] = "\\\\" + tokens[first];
}

vector<wstring> Help::File::TokenizePath(wstring path)
{
	vector<wstring> output;
	TokenizePath(path, output);
	return output;
}

vector<string> Help::File::TokenizePath(string path)
{
	vector<string> output;
	TokenizePath(path, output);
	return output;
}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP)

wstring Help::File::GetAbsolutePath(wstring path)
{
	if(IsAbsolutePath(path)) return path;

	wchar_t current[MAX_PATH+1];

	GetCurrentDirectoryW(MAX_PATH, current);
	current[MAX_PATH] = L'\0';

	vector<wstring> pathList;

	TokenizePath(current, pathList);
	TokenizePath(path, pathList);

	vector<wstring> parsed(pathList.size());

	int cursor = 0;

	for(auto& token : pathList)
	{
		if(token.empty() || token == L".")
			continue;

		if(token == L"..")
		{
			if(cursor > 1) cursor--;
			continue;
		}

		parsed[cursor] = token;
		cursor++;
	}	

	if(cursor == 0) return L"";

	wstringstream ss;
	ss << parsed[0];

	for(int i = 1;i < cursor;i++)
		ss << L'\\' << parsed[i];

	return ss.str();
}

string Help::File::GetAbsolutePath(string path)
{
	if(IsAbsolutePath(path)) return path;

	char current[MAX_PATH+1];

	GetCurrentDirectoryA(MAX_PATH, current);
	current[MAX_PATH] = '\0';

	vector<string> pathList;

	TokenizePath(current, pathList);
	TokenizePath(path, pathList);

	vector<string> parsed(pathList.size());

	int cursor = 0;

	for(auto& token : pathList)
	{
		if(token.empty() || token == ".")
			continue;

		if(token == "..")
		{
			if(cursor > 1) cursor--;
			continue;
		}

		parsed[cursor] = token;
		cursor++;
	}	

	if(cursor == 0) return "";

	stringstream ss;
	ss << parsed[0];

	for(int i = 1;i < cursor;i++)
		ss << '\\' << parsed[i];

	return ss.str();
}

wstring Help::File::GetRelativePath(wstring path)
{
	if(IsRelativePath(path)) return path;

	wchar_t current[MAX_PATH+1];

	GetCurrentDirectoryW(MAX_PATH, current);
	current[MAX_PATH] = L'\0';

	return GetRelativePath(path, current);
}

string Help::File::GetRelativePath(string path)
{
	if(IsRelativePath(path)) return path;

	char current[MAX_PATH+1];

	GetCurrentDirectoryA(MAX_PATH, current);
	current[MAX_PATH] = '\0';

	return GetRelativePath(path, current);
}

#endif

wstring Help::File::GetRelativePath(wstring path, wstring relativeToDirectory)
{
	if(IsRelativePath(path)) return path;

	auto currentList = TokenizePath(relativeToDirectory);

	vector<wstring> pathList;
	uint32_t commonCursor;

	TokenizePath(path, pathList);

	for(commonCursor = 0;commonCursor < min(pathList.size(), currentList.size()); commonCursor++)
		if(String::CompareCaseInsensitive(currentList[commonCursor], pathList[commonCursor]) != 0)
			break;

	wstringstream ss;

	bool first = true;

	for(uint32_t i = commonCursor;i < currentList.size();i++)
	{
		if(!first) ss << L"\\";
		first = false;
		ss << L"..";
	}

	for(uint32_t i = commonCursor;i < pathList.size();i++)
	{
		if(!first) ss << L"\\";
		first = false;
		ss << pathList[i];
	}

	return ss.str();
}

string Help::File::GetRelativePath(string path, string relativeToDirectory)
{
	if(IsRelativePath(path)) return path;

	auto currentList = TokenizePath(relativeToDirectory);

	vector<string> pathList;
	uint32_t commonCursor;

	TokenizePath(path, pathList);

	for(commonCursor = 0;commonCursor < min(pathList.size(), currentList.size()); commonCursor++)
		if(String::CompareCaseInsensitive(currentList[commonCursor], pathList[commonCursor]) != 0)
			break;

	stringstream ss;

	bool first = true;

	for(uint32_t i = commonCursor;i < currentList.size();i++)
	{
		if(!first) ss << "\\";
		first = false;
		ss << "..";
	}

	for(uint32_t i = commonCursor;i < pathList.size();i++)
	{
		if(!first) ss << "\\";
		first = false;
		ss << pathList[i];
	}

	return ss.str();
}

wstring Help::File::SimplifyPath(wstring path)
{
	bool absolute = IsAbsolutePath(path);
	auto tokens = TokenizePath(path);

	vector<wstring> output(tokens.size());
	int cursor = 0;

	for(auto& token : tokens)
	{
		if(token.empty() || token == L".")
			continue;

		if(token == L"..")
		{
			if(absolute)
			{
				if(cursor < 1)
					cursor--;
			}
			else
			{
				if(cursor > 0 && output[cursor-1] != L"..")
					cursor--;
				else
				{
					output[cursor] = token;
					cursor++;
				}
			}
		}
		else
		{
			output[cursor] = token;
			cursor++;
		}
	}


	if(cursor == 0) return L".";

	wstringstream ss;
	ss << output[0];

	for(int i = 1;i < cursor;i++)
		ss << L"\\" << output[i];

	return ss.str();
}

string Help::File::SimplifyPath(string path)
{
	bool absolute = IsAbsolutePath(path);
	auto tokens = TokenizePath(path);

	vector<string> output(tokens.size());
	int cursor = 0;

	for(auto& token : tokens)
	{
		if(token.empty() || token == ".")
			continue;

		if(token == "..")
		{
			if(absolute)
			{
				if(cursor < 1)
					cursor--;
			}
			else
			{
				if(cursor > 0 && output[cursor-1] != "..")
					cursor--;
				else
				{
					output[cursor] = token;
					cursor++;
				}
			}
		}
		else
		{
			output[cursor] = token;
			cursor++;
		}
	}


	if(cursor == 0) return ".";

	stringstream ss;
	ss << output[0];

	for(int i = 1;i < cursor;i++)
		ss << "\\" << output[i];

	return ss.str();
}

vector<wstring> Help::File::EnumerateFiles(wstring directoryPath)
{
    vector<wstring> names;

	if(*directoryPath.rbegin() == L'\\' || *directoryPath.rbegin() == L'/')
	    directoryPath += L"*.*";
	else
	    directoryPath += L"\\*.*";

    WIN32_FIND_DATAW fd; 
    HANDLE hFind = ::FindFirstFileExW(directoryPath.c_str(), FindExInfoStandard, &fd, FindExSearchNameMatch, nullptr, 0); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFileW(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}

vector<string> Help::File::EnumerateFiles(string directoryPath)
{
    vector<string> names;

	if(*directoryPath.rbegin() == '\\' || *directoryPath.rbegin() == '/')
	    directoryPath += "*.*";
	else
	    directoryPath += "\\*.*";

	WIN32_FIND_DATAA fd; 
    HANDLE hFind = ::FindFirstFileExA(directoryPath.c_str(), FindExInfoStandard, &fd, FindExSearchNameMatch, nullptr, 0); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFileA(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}

vector<wstring> Help::File::EnumerateDirectories(wstring parentDirectoryPath)
{
    vector<wstring> names;
	if(*parentDirectoryPath.rbegin() == L'\\' || *parentDirectoryPath.rbegin() == L'/')
	    parentDirectoryPath += L"*.*";
	else
	    parentDirectoryPath += L"\\*.*";

    WIN32_FIND_DATAW fd; 
    HANDLE hFind = ::FindFirstFileExW(parentDirectoryPath.c_str(), FindExInfoStandard, &fd, FindExSearchNameMatch, nullptr, 0); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFileW(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}

vector<string> Help::File::EnumerateDirectories(string parentDirectoryPath)
{
    vector<string> names;
	if(*parentDirectoryPath.rbegin() == '\\' || *parentDirectoryPath.rbegin() == '/')
	    parentDirectoryPath += "*.*";
	else
	    parentDirectoryPath += "\\*.*";

	WIN32_FIND_DATAA fd; 
    HANDLE hFind = ::FindFirstFileExA(parentDirectoryPath.c_str(), FindExInfoStandard, &fd, FindExSearchNameMatch, nullptr, 0); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFileA(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}

uint64_t Help::File::GetModifiedTime(wstring path)
{
	union
	{
		uint64_t time;
		LARGE_INTEGER filetime;
	};

	HANDLE fh = CreateFile2(path.c_str(), GENERIC_READ, 0, OPEN_EXISTING, nullptr);
	if(!fh) return (uint64_t)-1;

	FILE_BASIC_INFO fi;
	GetFileInformationByHandleEx(fh, FileBasicInfo, &fi, sizeof(FILE_BASIC_INFO));
	filetime = fi.LastWriteTime;

	CloseHandle(fh);
	return time;
}

uint64_t Help::File::GetModifiedTime(string path)
{
	union
	{
		uint64_t time;
		LARGE_INTEGER filetime;
	};

	HANDLE fh = CreateFile2(String::SToW(path).c_str(), GENERIC_READ, 0, OPEN_EXISTING, nullptr);
	if(!fh) return (uint64_t)-1;

	FILE_BASIC_INFO fi;
	GetFileInformationByHandleEx(fh, FileBasicInfo, &fi, sizeof(FILE_BASIC_INFO));
	filetime = fi.LastWriteTime;

	CloseHandle(fh);
	return time;
}

uint64_t Help::File::GetCreatedTime(wstring path)
{
	union
	{
		uint64_t time;
		LARGE_INTEGER filetime;
	};

	HANDLE fh = CreateFile2(path.c_str(), GENERIC_READ, 0, OPEN_EXISTING, nullptr);
	if(!fh) return (uint64_t)-1;

	FILE_BASIC_INFO fi;
	GetFileInformationByHandleEx(fh, FileBasicInfo, &fi, sizeof(FILE_BASIC_INFO));
	filetime = fi.CreationTime;

	CloseHandle(fh);
	return time;
}

uint64_t Help::File::GetCreatedTime(string path)
{
	union
	{
		uint64_t time;
		LARGE_INTEGER filetime;
	};

	HANDLE fh = CreateFile2(String::SToW(path).c_str(), GENERIC_READ, 0, OPEN_EXISTING, nullptr);
	if(!fh) return (uint64_t)-1;

	FILE_BASIC_INFO fi;
	GetFileInformationByHandleEx(fh, FileBasicInfo, &fi, sizeof(FILE_BASIC_INFO));
	filetime = fi.CreationTime;

	CloseHandle(fh);
	return time;
}

uint64_t Help::File::GetAccessedTime(wstring path)
{
	union
	{
		uint64_t time;
		LARGE_INTEGER filetime;
	};

	HANDLE fh = CreateFile2(path.c_str(), GENERIC_READ, 0, OPEN_EXISTING, nullptr);
	if(!fh) return (uint64_t)-1;

	FILE_BASIC_INFO fi;
	GetFileInformationByHandleEx(fh, FileBasicInfo, &fi, sizeof(FILE_BASIC_INFO));
	filetime = fi.LastAccessTime;

	CloseHandle(fh);
	return time;
}

uint64_t Help::File::GetAccessedTime(string path)
{
	union
	{
		uint64_t time;
		LARGE_INTEGER filetime;
	};

	HANDLE fh = CreateFile2(String::SToW(path).c_str(), GENERIC_READ, 0, OPEN_EXISTING, nullptr);
	if(!fh) return (uint64_t)-1;

	FILE_BASIC_INFO fi;
	GetFileInformationByHandleEx(fh, FileBasicInfo, &fi, sizeof(FILE_BASIC_INFO));
	filetime = fi.LastAccessTime;

	CloseHandle(fh);
	return time;
}
