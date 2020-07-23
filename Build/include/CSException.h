#pragma once

#include <exception>
#include <deque>
#include <string>

namespace CS
{
	class Exception : public std::exception, public std::nested_exception
	{
	public:
		Exception();
		Exception(HRESULT hr);
		Exception(string msg);
		Exception(HRESULT hr, string msg);
		
		const HRESULT HResult;
	};

	class ArgumentException : public Exception
	{
	public:
		ArgumentException(string argName) : Exception(E_INVALIDARG, argName) { }
		ArgumentException(string argName, string msg) : Exception(E_INVALIDARG, argName + ": " + msg), ArgumentName(argName) { }

		const string ArgumentName;

	protected:
		ArgumentException(HRESULT hr, string argName, string msg) : Exception(hr, argName + ": " + msg), ArgumentName(argName) { }
	};

	class ArgumentNullException : public ArgumentException
	{
	public:
		ArgumentNullException(string argName) : ArgumentException(E_POINTER, argName, "Cannot be null") { }
		ArgumentNullException(string argName, string msg) : ArgumentException(E_POINTER, argName, msg) { }
	};

	class InvalidOperationException : public Exception
	{
	public:
		InvalidOperationException() : Exception("Operation is not valid") { }
		InvalidOperationException(HRESULT hr) : Exception(hr, "Operation is not valid") { }
		InvalidOperationException(string msg) : Exception(msg) { }
		InvalidOperationException(HRESULT hr, string msg) : Exception(hr, msg) { }
	};

	class AccessDeniedException : public Exception
	{
	public:
		AccessDeniedException() : Exception("Access is denied.") { }
		AccessDeniedException(string msg) : Exception(E_ACCESSDENIED, msg) { }
	};

	class NullReferenceException : public Exception
	{
	public:
		NullReferenceException() : Exception("Object reference not set to an instance of an object.") { }
		NullReferenceException(string msg) : Exception(msg) { }
	};

	class NotImplementedException : public Exception
	{
	public:
		NotImplementedException() : Exception(E_NOTIMPL, "Method is not implemented.") { }
		NotImplementedException(string msg) : Exception(E_NOTIMPL, msg) { }
	};

	class InvalidCastException : public Exception
	{
	public:
		InvalidCastException() : Exception(E_NOINTERFACE, "Invalid cast.") { }
		InvalidCastException(string fromType, string toType) : Exception(E_NOINTERFACE, "Invalid cast. Cannot cast from " + fromType + " to " + toType) { }
	};
}