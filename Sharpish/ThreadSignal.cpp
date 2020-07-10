#include "Sharpish.h"
#include "ThreadSignal.h"

using namespace CS;

ThreadSignal::ThreadSignal(bool go) : ThreadSignal(SignalType::AllThreads, go)
{
}

ThreadSignal::ThreadSignal(SignalType type, bool go)
{
    int flags = 0;

    if (type == SignalType::AllThreads)
        flags |= CREATE_EVENT_MANUAL_RESET;
    if (go)
        flags |= CREATE_EVENT_INITIAL_SET;

	_handle = CreateEventEx(nullptr, nullptr, flags, EVENT_ALL_ACCESS);
}

ThreadSignal::ThreadSignal(ThreadSignal&& rhs) : _handle(rhs._handle)
{
	rhs._handle = nullptr;
}

ThreadSignal::~ThreadSignal()
{
	if (_handle)
		CloseHandle(_handle);
}

bool ThreadSignal::Await(int timeoutMilliseconds)
{
	return WaitForSingleObjectEx(_handle, (DWORD)timeoutMilliseconds, false) == WAIT_OBJECT_0;
}

void ThreadSignal::Go()
{
	SetEvent(_handle);
}

void ThreadSignal::Stop()
{
	ResetEvent(_handle);
}