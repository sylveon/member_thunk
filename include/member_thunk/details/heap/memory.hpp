#pragma once
#include <cstddef>
#include <cstdint>
#include <memoryapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>

#include "../../error/win32_error.hpp"

namespace member_thunk::details
{
	inline void flush_instruction_cache(void* ptr, std::size_t size)
	{
		if (!FlushInstructionCache(GetCurrentProcess(), ptr, size))
		{
			throw win32_error("FlushInstructionCache");
		}
	}

	inline void* virtual_alloc(void* address, std::size_t size, std::uint32_t type, std::uint32_t protection)
	{
		void* ptr = VirtualAlloc(address, size, type, protection);
		if (!ptr)
		{
			throw win32_error("VirtualAlloc");
		}

		return ptr;
	}

	inline std::uint32_t virtual_protect(void* ptr, std::size_t size, std::uint32_t protection)
	{
		DWORD old_protection;
		if (!VirtualProtect(ptr, size, protection, &old_protection))
		{
			throw win32_error("VirtualProtect");
		}

		return old_protection;
	}

	inline void virtual_free(void* ptr, std::size_t size, std::uint32_t free_type)
	{
		if (!VirtualFree(ptr, size, free_type))
		{
			throw win32_error("VirtualFree");
		}
	}
}
