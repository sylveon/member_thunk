#pragma once
#include <cstddef>
#include <heapapi.h>
#include <memory>
#include <minwindef.h>
#include <wil/resource.h>
#include <winerror.h>
#include <winnt.h>

#include "error.hpp"

namespace member_thunk::details
{
	using offset_t = unsigned char;

	template<std::size_t num>
	inline constexpr bool is_power_of_two_v = num && ((num & (num - 1)) == 0);

	template<std::size_t alignment>
#ifdef __cpp_concepts // MIGRATION: IDE concept support
		requires is_power_of_two_v<alignment>
#endif
	inline constexpr bool is_aligned_heapalloc_v = MEMORY_ALLOCATION_ALIGNMENT >= alignment;

	inline HANDLE get_executable_heap()
	{
		static const wil::unique_hheap executable_heap = []
		{
			if (wil::unique_hheap heap { HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0) })
			{
				return heap;
			}
			else
			{
				throw_last_error("HeapCreate failed");
			}
		}();

		return executable_heap.get();
	}

	inline void executable_free(void* ptr)
	{
		if (!HeapFree(get_executable_heap(), 0, ptr))
		{
			throw_last_error("HeapFree failed");
		}
	}

	template<std::size_t alignment>
#ifdef __cpp_concepts // MIGRATION: IDE concept support
		requires is_power_of_two_v<alignment>
#endif
	inline void* aligned_executable_alloc(std::size_t size)
	{
		std::size_t allocation_size = size;
		if constexpr (!is_aligned_heapalloc_v<alignment>)
		{
			allocation_size += alignment;
		}

		if (void* ptr = HeapAlloc(get_executable_heap(), 0, allocation_size))
		{
			if constexpr (!is_aligned_heapalloc_v<alignment>)
			{
				const auto unaligned_ptr = static_cast<offset_t*>(ptr);

				// Reserve some space to store the offset.
				std::size_t usable_space = allocation_size - sizeof(offset_t);
				ptr = unaligned_ptr + 1;

				if (std::align(alignment, size, ptr, usable_space))
				{
					const auto aligned_ptr = static_cast<offset_t*>(ptr);
					aligned_ptr[-1] = static_cast<offset_t>(aligned_ptr - unaligned_ptr);
				}
				else
				{
					executable_free(unaligned_ptr);
					throw_win32_error(ERROR_OUTOFMEMORY, "Failed to align pointer in allocated memory block");
				}
			}

			return ptr;
		}
		else
		{
			// HeapAlloc doesn't give us much info about why it fails, if
			// it happens just assume it's because we ran out of memory.
			throw_win32_error(ERROR_OUTOFMEMORY, "HeapAlloc failed");
		}
	}

	template<std::size_t alignment>
#ifdef __cpp_concepts // MIGRATION: IDE concept support
		requires is_power_of_two_v<alignment>
#endif
	inline void aligned_executable_free(void* ptr)
	{
		if (ptr)
		{
			const auto aligned_ptr = static_cast<offset_t*>(ptr);

			offset_t offset = 0;
			if constexpr (!is_aligned_heapalloc_v<alignment>)
			{
				offset = aligned_ptr[-1];
			}

			executable_free(aligned_ptr - offset);
		}
	}
}