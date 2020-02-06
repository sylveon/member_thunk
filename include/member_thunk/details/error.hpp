#pragma once
#include <errhandlingapi.h>
#include <minwindef.h>
#include <system_error>

namespace member_thunk::details
{
	[[noreturn]] inline void throw_win32_error(DWORD error, const char* message)
	{
		throw std::system_error(static_cast<int>(error), std::system_category(), message);
	}

	[[noreturn]] inline void throw_last_error(const char* message)
	{
		throw_win32_error(GetLastError(), message);
	}
}