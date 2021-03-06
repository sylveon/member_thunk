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
		static constexpr std::uint32_t executable_page_protection = PAGE_EXECUTE_READ | PAGE_TARGETS_INVALID;
#else
		static constexpr std::uint32_t executable_page_protection = PAGE_EXECUTE_READ;
#endif
	}

	template<typename T>
	page<T>::page(details::region<T>* parent, std::byte* address) :
		executable(false),
		size(parent->parent->layout.page_size),
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

	template<typename T>
	void page<T>::set_call_target([[maybe_unused]] bool valid)
	{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
		PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY policy;
		if (!GetProcessMitigationPolicy(GetCurrentProcess(), ProcessControlFlowGuardPolicy, &policy, sizeof(policy)))
		{
			throw win32_error("GetProcessMitigationPolicy");
		}

		if (policy.EnableControlFlowGuard)
		{
			std::vector<CFG_CALL_TARGET_INFO> target_info;
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
#endif
	}

	template<typename T>
	void page<T>::free()
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

	template<typename T>
	details::thunk* page<T>::new_thunk(void* that, void* func)
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

	template<typename T>
	details::thunk* page<T>::page_end() const noexcept
	{
		return reinterpret_cast<details::thunk*>(reinterpret_cast<std::byte*>(begin) + size);
	}

	template<typename T>
	std::size_t page<T>::byte_offset(details::thunk* thunk) const noexcept
	{
		return reinterpret_cast<std::byte*>(thunk) - reinterpret_cast<std::byte*>(begin);
	}

	template<typename T>
	bool page<T>::full() const noexcept
	{
		return end + 1 > page_end();
	}

	template<typename T>
	void page<T>::mark_executable()
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
