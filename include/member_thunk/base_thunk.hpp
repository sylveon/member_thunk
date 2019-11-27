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
					throw_last_error("HeapCreate failed");
				}
			}

			return thunk_heap.get();
		}

		[[noreturn]] static void throw_last_error(const char* message)
		{
			throw std::system_error({ static_cast<int>(GetLastError()), std::system_category() }, message);
		}

		// Disable copy and move for all implementations
		base_thunk(const base_thunk&) = delete;
		base_thunk& operator=(const base_thunk&) = delete;

	protected:
		base_thunk() = default;

		void flush(void* ptr, std::size_t size)
		{
			if (!FlushInstructionCache(GetCurrentProcess(), ptr, size))
			{
				throw_last_error("FlushInstructionCache failed");
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
				throw_last_error("HeapAlloc failed");
			}
		}

		void operator delete(void* ptr, std::size_t size) noexcept(false)
		{
			if (ptr)
			{
				if (!HeapFree(init_heap(), 0, ptr))
				{
					throw_last_error("HeapFree failed");
				}
			}
		}
	};

	template<typename Derived, typename Func>
	class crtp_base_thunk : public base_thunk
	{
	protected:
		void flush()
		{
			base_thunk::flush(static_cast<Derived*>(this), sizeof(Derived));
		}

	public:
		Func get_thunked_function() const
		{
			return reinterpret_cast<Func>(static_cast<const Derived*>(this));
		}
	};
}