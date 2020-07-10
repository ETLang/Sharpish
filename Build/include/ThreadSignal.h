#pragma once

namespace CS
{
	class ThreadSignal
	{
		HANDLE _handle;

		void operator =(const ThreadSignal& rhs) { }

	public:
		enum SignalType
		{
			AllThreads,
			OneThread,
		};

		ThreadSignal(bool go = true);
		ThreadSignal(SignalType type, bool go = true);
		ThreadSignal(ThreadSignal&& rhs);
		~ThreadSignal();

		bool Await(int timeoutMilliseconds = INFINITE);

		void Go();
		void Stop();

		HANDLE GetHandle() const { return _handle; }
	};
}