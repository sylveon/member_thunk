#pragma once
#include <errhandlingapi.h>
#include <heapapi.h>
#include <new>
#include <processthreadsapi.h>
#include <system_error>
#include <wil/resource.h>

namespace member_thunk
{
	class base_thunk
	{
		inline static wil::unique_hheap thunk_heap;

		static HANDLE init_heap()
		{
			if (!thunk_heap)
			{
				thunk_heap.reset(HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0));
				if (!thunk_heap)
				{
					throw std::bad_alloc();
				}
			}

			return thunk_heap.get();
		}


		// Disable copy and move for all implementers
		base_thunk(const base_thunk&) = delete;
		base_thunk& operator=(const base_thunk&) = delete;

	protected:
		base_thunk() = default;

		void flush(void* ptr, std::size_t size)
		{
			if (!FlushInstructionCache(GetCurrentProcess(), ptr, size))
			{
				throw std::system_error(
					{ static_cast<int>(GetLastError()), std::system_category() },
					"FlushInstructionCache failed."
				);
			}
		}

	public:
		void* operator new(std::size_t size)
		{
			void* ptr = HeapAlloc(init_heap(), 0, size);
			if (ptr)
			{
				return ptr;
			}
			else
			{
				throw std::bad_alloc();

			}
		}

		void operator delete(void* ptr, std::size_t size)
		{
			HeapFree(init_heap(), 0, ptr);
		}
	};
}