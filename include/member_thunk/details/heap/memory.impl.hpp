#pragma once
#include "./memory.hpp"
#include <memoryapi.h>
#include <processthreadsapi.h>
#include <sysinfoapi.h>

#include "../../error/invalid_memory_layout.hpp"
#include "../../error/win32_error.hpp"
#include "region.hpp"

namespace member_thunk::details
{
	inline memory_layout memory_layout::get_for_current_system()
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info); // never fails

		if (info.dwAllocationGranularity % info.dwPageSize != 0 || info.dwAllocationGranularity / info.dwPageSize != pages_per_region)
		{
			throw invalid_memory_layout();
		}

		return { .page_size = info.dwPageSize, .allocation_granularity = info.dwAllocationGranularity };
	}

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
