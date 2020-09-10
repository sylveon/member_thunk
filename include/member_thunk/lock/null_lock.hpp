#pragma once

namespace member_thunk
{
	class null_lock final
	{
	public:
		constexpr null_lock() noexcept = default;
		null_lock(const null_lock&) = delete;
		null_lock& operator=(const null_lock&) = delete;

		void lock() noexcept { }
		void unlock() noexcept { }
	};
}