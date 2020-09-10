#pragma once
#include <synchapi.h>

namespace member_thunk
{
	class slim_lock final
	{
		SRWLOCK srwlock = SRWLOCK_INIT;

	public:
		constexpr slim_lock() noexcept = default;
		slim_lock(const slim_lock&) = delete;
		slim_lock& operator=(const slim_lock&) = delete;

		void lock() noexcept
		{
			AcquireSRWLockExclusive(&srwlock);
		}

		void unlock() noexcept
		{
			ReleaseSRWLockExclusive(&srwlock);
		}
	};
}