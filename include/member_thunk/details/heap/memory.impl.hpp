#pragma once
#include "./memory.hpp"
#include <memoryapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <sysinfoapi.h>
#include <winapifamily.h>

#include "../../error/invalid_memory_layout.hpp"
#include "../../error/win32_error.hpp"
#include "region.hpp"

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#	define MEMBER_THUNK_VIRTUAL_ALLOC VirtualAlloc
#	define MEMBER_THUNK_VIRTUAL_PROTECT VirtualProtect
#else
#	define MEMBER_THUNK_VIRTUAL_ALLOC VirtualAllocFromApp
#	define MEMBER_THUNK_VIRTUAL_PROTECT VirtualProtectFromApp
#endif

#define MEMBER_THUNK_STRINGIFY_INNER(x) #x
#define MEMBER_THUNK_STRINGIFY(x) MEMBER_THUNK_STRINGIFY_INNER(x)

namespace member_thunk::details
{
	inline memory_layout memory_layout::get_for_current_system()
	{
		static const auto layout = []
		{
			SYSTEM_INFO info;
			GetSystemInfo(&info); // never fails

			if (info.dwAllocationGranularity % info.dwPageSize != 0 || info.dwAllocationGranularity / info.dwPageSize != pages_per_region)
			{
				throw invalid_memory_layout();
			}

			return memory_layout { .page_size = info.dwPageSize, .allocation_granularity = info.dwAllocationGranularity };
		}();

		return layout;
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
		void* ptr = MEMBER_THUNK_VIRTUAL_ALLOC(address, size, type, protection);
		if (!ptr)
		{
			throw win32_error(MEMBER_THUNK_STRINGIFY(MEMBER_THUNK_VIRTUAL_ALLOC));
		}

		return ptr;
	}

	inline std::uint32_t virtual_protect(void* ptr, std::size_t size, std::uint32_t protection)
	{
		DWORD old_protection;
		if (!MEMBER_THUNK_VIRTUAL_PROTECT(ptr, size, protection, &old_protection))
		{
			throw win32_error(MEMBER_THUNK_STRINGIFY(MEMBER_THUNK_VIRTUAL_PROTECT));
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

#undef MEMBER_THUNK_STRINGIFY
#undef MEMBER_THUNK_STRINGIFY_INNER
#undef MEMBER_THUNK_VIRTUAL_PROTECT
#undef MEMBER_THUNK_VIRTUAL_ALLOC
