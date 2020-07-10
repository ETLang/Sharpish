#pragma once

//////////////////////////////////////////////////////////////////////////
//  CriticalSection
//  Description: Wraps a critical section.
//////////////////////////////////////////////////////////////////////////

namespace CS
{
	class CriticalSection
	{
	public:
		CRITICAL_SECTION _criticalSection;
	public:
		CriticalSection(CriticalSection&& rh)
		{
			_criticalSection = rh._criticalSection;
			memset((void*)&rh._criticalSection, 0, sizeof(CRITICAL_SECTION));
		}

		CriticalSection()
		{
			InitializeCriticalSectionEx(&_criticalSection, 100, 0);
		}

		~CriticalSection()
		{
			DeleteCriticalSection(&_criticalSection);
		}

		_Acquires_lock_(_criticalSection)
			void Lock()
		{
			EnterCriticalSection(&_criticalSection);
		}

		_Releases_lock_(_criticalSection)
			void Unlock()
		{
			LeaveCriticalSection(&_criticalSection);
		}
	};


	//////////////////////////////////////////////////////////////////////////
	//  AutoLock
	//  Description: Provides automatic locking and unlocking of a 
	//               of a critical section.
	//
	//  Note: The AutoLock object must go out of scope before the CriticalSection.
	//////////////////////////////////////////////////////////////////////////

	class AutoLock
	{
	private:
		CriticalSection *_pCriticalSection;
	public:
        _Acquires_lock_(_pCriticalSection)
            AutoLock(const CriticalSection& crit)
        {
            _pCriticalSection = const_cast<CriticalSection*>(&crit);
            _pCriticalSection->Lock();
        }

		_Acquires_lock_(_pCriticalSection)
			AutoLock(CriticalSection& crit)
		{
			_pCriticalSection = &crit;
			_pCriticalSection->Lock();
		}

		_Releases_lock_(_pCriticalSection)
			~AutoLock()
		{
			_pCriticalSection->Unlock();
		}
	};
}