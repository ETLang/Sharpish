#pragma once

#include "StringHelper.h"

namespace CS
{
	namespace Help
	{
		class File
		{
		public:
			static std::vector<byte> ReadWhole(std::wstring path);
			static std::vector<byte> ReadWhole(std::string path);

			static void WriteWhole(std::wstring path, std::wstring contents);
			static void WriteWhole(std::string path, std::string contents);

			static void AppendTo(std::wstring path, std::wstring contents);
			static void AppendTo(std::string path, std::string contents);

			static void Delete(std::string path);
			static void Delete(std::wstring path);

			static bool FileExists(std::wstring path);
			static bool FileExists(std::string path) { return FileExists(String::SToW(path)); }

			static bool DirectoryExists(const std::wstring& path);
			static bool DirectoryExists(const std::string& path) { return DirectoryExists(String::SToW(path)); }

			static size_t GetSize(std::wstring path);
			static size_t GetSize(std::string path);

			static std::wstring GetDrive(std::wstring path);
			static std::string GetDrive(std::string path);

			static std::wstring GetContainingDirectory(std::wstring path);
			static std::string GetContainingDirectory(std::string path);

			static std::wstring GetName(std::wstring path);
			static std::string GetName(std::string path);

			static std::wstring GetNameNoExtension(std::wstring path);
			static std::string GetNameNoExtension(std::string path);

			static std::wstring GetExtension(std::wstring path);
			static std::string GetExtension(std::string path);

			static std::wstring CombinePath(std::wstring a, std::wstring b);
			static std::string CombinePath(std::string a, std::string b);

			static bool IsRelativePath(std::wstring path);
			static bool IsRelativePath(std::string path);

			static void TokenizePath(std::wstring path, std::vector<std::wstring>& tokens);
			static void TokenizePath(std::string path, std::vector<std::string>& tokens);

			static std::vector<std::wstring> TokenizePath(std::wstring path);
			static std::vector<std::string> TokenizePath(std::string path);

			static bool IsAbsolutePath(std::wstring path);
			static bool IsAbsolutePath(std::string path);

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_PHONE_APP)

			static std::wstring GetAbsolutePath(std::wstring path);
			static std::string GetAbsolutePath(std::string path);

			static std::wstring GetRelativePath(std::wstring path);
			static std::string GetRelativePath(std::string path);

#endif

			static std::wstring GetRelativePath(std::wstring path, std::wstring relativeToDirectory);
			static std::string GetRelativePath(std::string path, std::string relativeToDirectory);

			static std::wstring SimplifyPath(std::wstring path);
			static std::string SimplifyPath(std::string path);

			static std::vector<std::wstring> EnumerateFiles(std::wstring directoryPath);
			static std::vector<std::string> EnumerateFiles(std::string directoryPath);

			static std::vector<std::wstring> EnumerateDirectories(std::wstring parentDirectoryPath);
			static std::vector<std::string> EnumerateDirectories(std::string parentDirectoryPath);

			static uint64_t GetModifiedTime(std::wstring path);
			static uint64_t GetModifiedTime(std::string path);

			static uint64_t GetCreatedTime(std::wstring path);
			static uint64_t GetCreatedTime(std::string path);

			static uint64_t GetAccessedTime(std::wstring path);
			static uint64_t GetAccessedTime(std::string path);
		};
	}
}