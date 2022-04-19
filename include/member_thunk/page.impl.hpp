#pragma once
#include "./page.hpp"
#include <algorithm>
#include <iterator>
#include <memory>
#include <memoryapi.h>
#include <processthreadsapi.h>
#include <vector>
#include <winapifamily.h>

#include "./details/heap/memory.hpp"
#include "./details/heap/region.hpp"
#include "./details/thunk/thunk.hpp"
#include "./error/page_executable.hpp"
#include "./error/page_full.hpp"
#include "./error/win32_error.hpp"
#include "./heap.hpp"

namespace member_thunk
{
	namespace details
	{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
		static constexpr std::uint32_t executable_page_protection = PAGE_EXECUTE_READ | PAGE_TARGETS_NO_UPDATE;
#else
		// Since UWP doesn't allow us to specify an executable page protection with VirtualAlloc, we can't pass
		// PAGE_TARGETS_NO_UPDATE here because the CFG bitmap would never get initialized, making
		// SetProcessValidCallTargets fail with an invalid parameter error.
		// This is okay, since once SetProcessValidCallTargets is called, it won't be possible
		// to call invalid targets anymore.
		// It does mean there is a slight interval where it is possible to call invalid targets, but better than nothing.
		static constexpr std::uint32_t executable_page_protection = PAGE_EXECUTE_READ;
#endif
	}

	page::page(details::abstract_region* parent, std::byte* address) :
		executable(false),
		size(parent->page_size()),
		parent(parent),
		begin(static_cast<details::thunk*>(details::virtual_alloc(address, size, MEM_COMMIT, PAGE_READWRITE))),
		end(begin)
	{
		// fill the entire page with debug breaks by creating a thunk and then destroying it.
		// this is required because zeroes are valid instructions in x64.
		std::ranges::for_each(begin, page_end(),
			[](details::thunk& thunk) noexcept
			{
				(new (&thunk) details::thunk())->~thunk();
			});
	}

	void page::set_call_target(bool valid)
	{
		PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY policy;
		if (!GetProcessMitigationPolicy(GetCurrentProcess(), ProcessControlFlowGuardPolicy, &policy, sizeof(policy)))
		{
			throw win32_error("GetProcessMitigationPolicy");
		}

		if (policy.EnableControlFlowGuard)
		{
			std::vector<CFG_CALL_TARGET_INFO> target_info;
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
			// UWP needs a first pass disabling every 16-byte aligned address in the page.
			target_info.resize(size / 16);

			std::ranges::generate(target_info,
				[n = 0ULL]() mutable
				{
					return CFG_CALL_TARGET_INFO {
						.Offset = 16 * n++,
						.Flags = 0,
					};
				});

			if (!SetProcessValidCallTargets(GetCurrentProcess(), begin, size, static_cast<ULONG>(target_info.size()), target_info.data()))
			{
				throw win32_error("SetProcessValidCallTargets");
			}

			target_info.clear();
#endif

			target_info.reserve(end - begin);

			std::ranges::transform(begin, end, std::back_inserter(target_info),
				[this, valid](details::thunk& thunk) noexcept
				{
					return CFG_CALL_TARGET_INFO {
						.Offset = static_cast<ULONG_PTR>(byte_offset(&thunk)),
						.Flags = static_cast<ULONG_PTR>(valid ? CFG_CALL_TARGET_VALID : 0),
					};
				});

			if (!SetProcessValidCallTargets(GetCurrentProcess(), begin, size, static_cast<ULONG>(target_info.size()), target_info.data()))
			{
				throw win32_error("SetProcessValidCallTargets");
			}
		}
	}

	void page::free()
	{
		if (executable)
		{
			set_call_target(false);
			details::virtual_protect(begin, size, PAGE_READWRITE);
		}

		std::ranges::destroy(begin, end);

		if (executable)
		{
			details::flush_instruction_cache(begin, byte_offset(end));
		}

		details::virtual_free(begin, size, MEM_DECOMMIT);
		parent->mark_decommited(reinterpret_cast<std::byte*>(begin));
	}

	details::thunk* page::new_thunk(void* that, void* func)
	{
		if (executable)
		{
			throw page_executable();
		}

		if (full())
		{
			throw page_full();
		}

		return new (end++) details::thunk(that, func);
	}

	details::thunk* page::page_end() const noexcept
	{
		return reinterpret_cast<details::thunk*>(reinterpret_cast<std::byte*>(begin) + size);
	}

	std::size_t page::byte_offset(details::thunk* thunk) const noexcept
	{
		return reinterpret_cast<std::byte*>(thunk) - reinterpret_cast<std::byte*>(begin);
	}

	bool page::full() const noexcept
	{
		return end + 1 > page_end();
	}

	void page::mark_executable()
	{
		if (!executable)
		{
			details::flush_instruction_cache(begin, byte_offset(end));
			details::virtual_protect(begin, size, details::executable_page_protection);
			executable = true;

			set_call_target(true);
		}
	}
}
