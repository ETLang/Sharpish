#pragma once

#include "ComClass.h"

// WeakReference implementation, originally by Microsoft. Minor modifications were made
// to make it work with Sharpish's object model internally.

namespace CS
{
	namespace Details
	{

#if defined(_X86_) || defined(_AMD64_) || defined(_ARM64_)

#define UnknownIncrementReference InterlockedIncrement
#define UnknownDecrementReference InterlockedDecrement
#define UnknownBarrierAfterInterlock() 

#elif defined(_ARM_)

#define UnknownIncrementReference InterlockedIncrementNoFence
#define UnknownDecrementReference InterlockedDecrementRelease
#define UnknownBarrierAfterInterlock() __dmb(_ARM_BARRIER_ISH)

#endif

		//Weak reference implementation
		class WeakReference : public ComInstance<Ext::IWeakReference>
		{
		public:
			WeakReference() throw() : _unknown(nullptr), _strongRefCount(1)
			{
			}

			virtual ~WeakReference() throw()
			{
			}

			STDMETHOD(Resolve)(REFIID riid, void**ppvObject)    
			{            
				*ppvObject = nullptr;

				for(;;)
				{
					long ref = _strongRefCount;
					if (ref == 0)
					{
						return S_OK;
					}

					// InterlockedCompareExchange calls _InterlockedCompareExchange intrinsic thus we call directly _InterlockedCompareExchange to save the call
					if (::_InterlockedCompareExchange(&_strongRefCount, ref + 1, ref) == ref)
					{
		#ifdef _PERF_COUNTERS
						// This artificially manipulates the strong ref count via AddRef to account for the resolve 
						// interlocked operation above when tallying reference counting operations.
						_unknown->AddRef();
						::_InterlockedDecrement(&_strongRefCount);
		#endif
						break;
					}
				}
            
				HRESULT hr = _unknown->QueryInterface(riid, reinterpret_cast<void**>(ppvObject));
				_unknown->Release();
				return hr;
			}

			unsigned long IncrementStrongReference() throw()
			{
				return UnknownIncrementReference(&_strongRefCount);
			}

			unsigned long DecrementStrongReference() throw()
			{
				// A release fence is required to ensure all guarded memory accesses are
				// complete before any thread can begin destroying the object.
				unsigned long newValue = UnknownDecrementReference(&_strongRefCount);
				if (newValue == 0)
				{
					// An acquire fence is required before object destruction to ensure
					// that the destructor cannot observe values changing on other threads.
					UnknownBarrierAfterInterlock();
				}
				return newValue;
			}

			unsigned long GetStrongRefCount() const throw()
			{
				return _strongRefCount;
			}

			void SetUnknown(IUnknown* unk) throw()
			{
				_unknown = unk;
			}

		private:
			volatile long _strongRefCount;    
			IUnknown *_unknown;
		};
	}
}