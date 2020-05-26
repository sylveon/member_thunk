#pragma once
#include <bit>
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

	template<std::size_t alignment>
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

	inline void* executable_alloc(std::size_t size)
	{
		if (void* ptr = HeapAlloc(get_executable_heap(), 0, size))
		{
			return ptr;
		}
		else
		{
			// HeapAlloc doesn't give us much info about why it fails, if
			// it happens just assume it's because we ran out of memory.
			throw_win32_error(ERROR_OUTOFMEMORY, "HeapAlloc failed");
		}
	}

	inline void executable_free(void* ptr)
	{
		if (ptr && !HeapFree(get_executable_heap(), 0, ptr))
		{
			throw_last_error("HeapFree failed");
		}
	}

	template<std::size_t alignment>
#ifdef __cpp_lib_bitops // MIGRATION: MSVC <bit> support
	requires is_aligned_heapalloc_v<alignment> && (std::has_single_bit(alignment))
#else
	requires is_aligned_heapalloc_v<alignment> && (alignment != 0 && (alignment & (alignment - 1)) == 0)
#endif
	inline void* aligned_executable_alloc(std::size_t size)
	{
		const std::size_t allocation_size = size + alignment;
		const auto unaligned_ptr = static_cast<offset_t*>(executable_alloc(allocation_size));

		// Reserve some space to store the offset.
		std::size_t usable_space = allocation_size - sizeof(offset_t);
		void* ptr = unaligned_ptr + 1;

		if (std::align(alignment, size, ptr, usable_space))
		{
			const auto aligned_ptr = static_cast<offset_t*>(ptr);
			aligned_ptr[-1] = static_cast<offset_t>(aligned_ptr - unaligned_ptr);

			return ptr;
		}
		else
		{
			executable_free(unaligned_ptr);
			throw_win32_error(ERROR_OUTOFMEMORY, "Failed to align pointer in allocated memory block");
		}
	}

	inline void aligned_executable_free(void* ptr)
	{
		if (ptr)
		{
			const auto aligned_ptr = static_cast<offset_t*>(ptr);
			executable_free(aligned_ptr - aligned_ptr[-1]);
		}
	}
}