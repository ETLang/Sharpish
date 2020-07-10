#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <wchar.h>

namespace CS
{
	namespace Help
	{
		class String
		{
			static const char* _Whitespace;
			static const wchar_t* _WWhitespace;

		public:

			static std::wstring ConstCopy(const std::wstring& s) { return std::wstring(&s[0], s.size()); }
			static std::string ConstCopy(const std::string& s) { return std::string(&s[0], s.size()); }

			static int CompareCaseInsensitive(const std::wstring& a, const std::wstring& b) { return _wcsicmp(a.c_str(), b.c_str()); }
			static int CompareCaseInsensitive(const std::string& a, const std::string& b) { return _stricmp(a.c_str(), b.c_str()); }

			static std::wstring SToW(const std::string& s);
			static std::wstring SToW(const char* s) { return SToW(std::string(s)); }
			static std::string WToS(const std::wstring& s);
			static std::string WToS(const wchar_t* s) { return WToS(std::wstring(s)); }

			// trim from end of string (right)
			static inline std::string& TrimEnd(std::string& s, const char* t = _Whitespace)
			{
				s.erase(s.find_last_not_of(t) + 1);
				return s;
			}

			static inline std::wstring& TrimEnd(std::wstring& s, const wchar_t* t = _WWhitespace)
			{
				s.erase(s.find_last_not_of(t) + 1);
				return s;
			}

			// trim from beginning of string (left)
			static inline std::string& TrimStart(std::string& s, const char* t = _Whitespace)
			{
				s.erase(0, s.find_first_not_of(t));
				return s;
			}

			static inline std::wstring& TrimStart(std::wstring& s, const wchar_t* t = _WWhitespace)
			{
				s.erase(0, s.find_first_not_of(t));
				return s;
			}

			// trim from both ends of string (left & right)
			static inline std::string& Trim(std::string& s, const char* t = _Whitespace)
			{
				return TrimStart(TrimEnd(s, t), t);
			}

			static inline std::wstring& Trim(std::wstring& s, const wchar_t* t = _WWhitespace)
			{
				return TrimStart(TrimEnd(s, t), t);
			}

			static inline bool StartsWith(const std::string& str, const std::string& prefix)
			{
				return str.compare(0, prefix.size(), prefix) == 0;
			}

			static inline bool StartsWith(const std::wstring& str, const std::wstring& prefix)
			{
				return str.compare(0, prefix.size(), prefix) == 0;
			}

			static inline bool Contains(const std::string& str, const std::string& sub)
			{
				return str.find(sub) != std::string::npos;
			}

			static inline bool Contains(const std::wstring& str, const std::wstring& sub)
			{
				return str.find(sub) != std::wstring::npos;
			}

			static inline bool EndsWith(const std::string& str, const std::string& end)
			{
				auto endsize = end.size();
				return str.compare(str.size() - endsize, endsize, end) == 0;
			}

			static inline bool EndsWith(const std::wstring& str, const std::wstring& end)
			{
				auto endsize = end.size();
				return str.compare(str.size() - endsize, endsize, end) == 0;
			}

			static inline std::wstring ToUpper(const std::wstring& s)
			{
				std::wstring output(s.size(), L' ');

				transform(
				  s.begin(), s.end(),
				  output.begin(),
				  towupper);

				return output;
			}

			static inline std::string ToUpper(const std::string& s)
			{
                std::string output(s.size(), ' ');

				transform(
				  s.begin(), s.end(),
				  output.begin(),
				  toupper);

				return output;
			}

			static inline std::wstring ToLower(const std::wstring& s)
			{
				std::wstring output(s.size(), L' ');

				transform(
				  s.begin(), s.end(),
				  output.begin(),
				  towlower);

				return output;
			}

			static inline std::string ToLower(const std::string& s)
			{
                std::string output(s.size(), ' ');

				transform(
				  s.begin(), s.end(),
				  output.begin(),
				  tolower);

				return output;
			}

			static std::vector<std::wstring>& Split(const std::wstring &s, wchar_t delim, std::vector<std::wstring> &elems)
			{
				std::wstringstream ss(s);
				std::wstring item;
				while (getline(ss, item, delim)) {
					elems.push_back(item);
				}
				return elems;
			}


			static std::vector<std::wstring> Split(const std::wstring &s, wchar_t delim)
			{
                std::vector<std::wstring> elems;
				Split(s, delim, elems);
				return elems;
			}

			static std::vector<std::string>& Split(const std::string &s, char delim, std::vector<std::string> &elems)
			{
                std::stringstream ss(s);
                std::string item;
				while (getline(ss, item, delim)) {
					elems.push_back(item);
				}
				return elems;
			}


			static std::vector<std::string> Split(const std::string &s, char delim)
			{
                std::vector<std::string> elems;
				Split(s, delim, elems);
				return elems;
			}

			static std::string Replace(const std::string& str, const std::string& find, const std::string& sub)
			{
				if(str.empty())
					return str;
				if(find.empty())
					return str;

				size_t start_pos = 0;
                std::string output = str;
				while((start_pos = output.find(find, start_pos)) != std::string::npos) {
					output.replace(start_pos, find.length(), sub);
					start_pos += sub.length();
				}

				return output;
			}

			static std::wstring Replace(const std::wstring& str, const std::wstring& find, const std::wstring& sub) 
			{
				if(str.empty())
					return str;
				if(find.empty())
					return str;

				size_t start_pos = 0;
				std::wstring output = str;
				while((start_pos = output.find(find, start_pos)) != std::wstring::npos) {
					output.replace(start_pos, find.length(), sub);
					start_pos += sub.length();
				}

				return output;
			}
		
			template<typename T>
			static int FindCaseInsensitive(const T &haystack, const T &needle)
			{
				auto it = std::search(
					haystack.begin(), haystack.end(),
					needle.begin(),   needle.end(),
					[](typename T::value_type ch1, typename T::value_type ch2) { return toupper(ch1) == toupper(ch2); });
				return (it == haystack.end()) ? -1 : (int)(it - haystack.begin());
			}

            static com_ptr<IBlob> AsBlob(std::string str);
            static com_ptr<IBlob> AsBlob(std::wstring str);
            static string FromBlob(IBlob* blob);

			static std::wstring D3DUnblob(IBlob* blob);

			static UUID GenerateUUIDFromString(std::string str);
			static UUID GenerateUUIDFromString(std::wstring str);

			static std::wstring Format(std::wstring format, ...);
			static std::string Format(std::string format, ...);
		};
	}
}