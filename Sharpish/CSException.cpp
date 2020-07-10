#include "Sharpish.h"
#include "StringHelper.h"

using namespace CS;

Exception::Exception() : std::exception(), HResult(S_OK) { }
Exception::Exception(HRESULT hr) : std::exception(), HResult(hr) { }
Exception::Exception(string msg) : std::exception(msg.c_str()), HResult(S_OK) { }
Exception::Exception(HRESULT hr, string msg) : std::exception(msg.c_str()), HResult(hr) { }

#ifdef _DEPRECATED_

#pragma warning(disable : 4996)

namespace CS
{
	char Exception::_ExceptionContextBuffer[Exception::MaxBufferLength] = "";

	Exception::Exception(const char *msg, ...) throw()
		: _genmsg(nullptr),
		_shouldFreeMsg(false),
		_inner(nullptr),
		_result(S_OK),
		_msg(new char[Exception::MaxBufferLength])
	{
		va_list args;
		if (msg)
		{
			va_start(args, msg);
			vswprintf(_msg, msg, args);
			va_end(args);
		}
		else
			*_msg = 0;

		InitCallStack();
		DumpToContextBuffer();
		FirstChanceReport();
	}

	Exception::Exception(std::exception &inner, const char *msg, ...) throw()
		: _genmsg(nullptr),
		_shouldFreeMsg(false),
		_inner(&inner),
		_result(S_OK),
		_msg(new char[Exception::MaxBufferLength])
	{
		va_list args;

		if (msg)
		{
			va_start(args, msg);
			vswprintf(_msg, msg, args);
			va_end(args);
		}
		else
			*_msg = 0;

		InitCallStack();
		DumpToContextBuffer();
		FirstChanceReport();
	}

	Exception::Exception(HRESULT result, const char *msg, ...) throw()
		: _genmsg(nullptr),
		_shouldFreeMsg(true),
		_inner(nullptr),
		_result(result),
		_msg(new char[Exception::MaxBufferLength])
	{
		va_list args;

		if (msg)
		{
			va_start(args, msg);
			vswprintf(_msg, msg, args);
			va_end(args);
		}
		else
			*_msg = 0;

		ReadHResult(result, msg);
		InitCallStack();
		DumpToContextBuffer();
		FirstChanceReport();
	}

	Exception::Exception(std::exception &inner, HRESULT result, const char *msg, ...) throw()
		: _genmsg(nullptr),
		_shouldFreeMsg(true),
		_inner(&inner),
		_result(result),
		_msg(new char[Exception::MaxBufferLength])
	{
		va_list args;

		if (msg)
		{
			va_start(args, msg);
			vswprintf(_msg, msg, args);
			va_end(args);
		}
		else
			*_msg = 0;

		ReadHResult(result, msg);
		InitCallStack();
		DumpToContextBuffer();
		FirstChanceReport();
	}

	Exception::Exception(const Exception &copy) throw()
		: _shouldFreeMsg(copy._shouldFreeMsg),
		_inner(copy._inner),
		_result(copy._result),
		_msg(new char[Exception::MaxBufferLength])
	{
		memcpy(_msg, copy._msg, sizeof(char)* 65536);

		if (copy._genmsg != nullptr)
		{
			_genmsg = new char[wcslen(copy._genmsg) + 1];
			wcscpy(_genmsg, copy._genmsg);
		}

		_callStack = copy._callStack;
	}

	Exception::~Exception()
	{
		if(_genmsg) delete _genmsg;
		delete [] _msg;
	}

	void Exception::InitCallStack()
	{
#ifdef _DEBUG
		// TODO figure out what happened to stackwalker. It seems to have broken in VS2012.
		//_callStack = EVEStackWalker().GetCallStack();
#endif
	}

	void Exception::ReadHResult(HRESULT err, const char *msg)
	{
		const char *simplestuff = "HR: 0x";
		char *text = new char[32768];

		// Format:
		// <msg>\r\n(HR 0xHHHHHHHH: <text>)

		int success = ::FormatMessage(
			/*FORMAT_MESSAGE_ALLOCATE_BUFFER|*/FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			text,
			32768,
			NULL);

		_shouldFreeMsg = true;
		if (success)
		{
			if (msg)
			{
				auto size = (wcslen(msg) + 9 + 10 + wcslen(text) + 2);
				_genmsg = new char[size];
				_swprintf(_genmsg, "%s\r\n(HR: 0x%08X: %s)", msg, err, text);
			}
			else
			{
				auto size = (6 + 10 + wcslen(text) + 1);
				_genmsg = new char[size];
				_swprintf(_genmsg, "HR: 0x%08X: %s", err, text);
			}
		}
		else
		{
			if (msg)
			{
				auto size = wcslen(msg) + 9 + 10 + 2;
				_genmsg = new char[size];
				_swprintf(_genmsg, "%s\r\n(HR: 0x%08X)", msg, err);
			}
			else
			{
				int size = 6 + 10 + 1;
				_genmsg = new char[size];
				_swprintf(_genmsg, "HR: 0x%08X", err);
			}
		}

		delete [] text;
	}

	char *Exception::Append(char *buf, int *size, int strings, ...)
	{
		if (*size == 0) return buf;

		va_list vl;
		va_start(vl, strings);

		for (int i = 0; i < strings; i++)
		{
			const char *str = va_arg(vl, const char *);

			auto strlength = wcslen(str);

			wcscpy_s(buf, *size, str);

			if (*size < strlength)
			{
				buf += *size;
				*size = 0;
				break;
			}

			buf += strlength;
			*size -= (int)strlength;
		}

		va_end(vl);
		return buf;
	}

	void Exception::DumpToContextBuffer(char *buffer, int size, int indent)
	{
		// Convert all the exception data into a nice string message
		char *indentstr = new char[indent * 3 + 1];
		memset(indentstr, ' ', indent * 3);
		indentstr[indent * 3] = '\0';

		buffer = Append(buffer, &size, 3, indentstr, (*_msg == 0) ? _genmsg : _msg, "\r\n");
		if (_inner)
		{
			buffer = Append(buffer, &size, 2, indentstr, "Inner Exception:\r\n");
			Exception *ie = dynamic_cast<Exception*>(_inner);

			if (ie)
				ie->DumpToContextBuffer(buffer, size, indent + 1);
			else
				buffer = Append(buffer, &size, 2, indentstr, _inner->what());

			buffer = Append(buffer, &size, 2, indentstr, "-- End Inner Exception\r\n");
		}

#ifdef _DEBUG
		if (!_callStack.empty())
		for (deque<string>::iterator i = _callStack.begin() + 4; i != _callStack.end(); i++)
		{
			buffer = Append(buffer, &size, 2, indentstr, i->c_str());
		}
#endif

		delete[] indentstr;
	}

	void Exception::FirstChanceReport()
	{
		OutputDebugStringW("::: Internal Sharpish Exception!\r\n::: ");
		OutputDebugStringW(_ExceptionContextBuffer);

		if(IsDebuggerPresent())
			_CrtDbgBreak(); // Check your output window for exception details
	}
}

#endif