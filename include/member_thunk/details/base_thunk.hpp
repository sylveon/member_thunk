#pragma once
#include <cstddef>
#include <memoryapi.h>
#include <minwindef.h>
#include <new>
#include <processthreadsapi.h>

#include "error.hpp"
#include "memory.hpp"

#define MEMBER_THUNK_PACK _Pragma("pack(push, 1)")
#define MEMBER_THUNK_UNPACK _Pragma("pack(pop)")

namespace member_thunk::details
{
	template<typename Derived, typename Func>
	class alignas(16) base_thunk
	{
		// Disable copy and move for all implementations
		base_thunk(const base_thunk&) = delete;
		base_thunk& operator=(const base_thunk&) = delete;

		void flush()
		{
			if (!FlushInstructionCache(GetCurrentProcess(), static_cast<Derived*>(this), sizeof(Derived)))
			{
				throw_last_error("FlushInstructionCache failed");
			}
		}

		void set_call_target(bool valid)
		{
			auto info = CFG_CALL_TARGET_INFO
			{
				.Offset = 0,
				.Flags = static_cast<ULONG_PTR>(valid ? CFG_CALL_TARGET_VALID : 0)
			};

			if (!SetProcessValidCallTargets(GetCurrentProcess(), static_cast<Derived*>(this), sizeof(Derived), 1, &info))
			{
				throw_last_error("SetProcessValidCallTargets failed");
			}
		}

	protected:
		base_thunk() = default;

		template<std::size_t size>
		void init_thunk()
		{
			static_assert(sizeof(Derived) == size, "Thunk class does not have expected size");
			static_assert(alignof(Derived) <= alignof(base_thunk), "Thunk class does not have expected alignment");

			flush();
			set_call_target(true);
		}

		~base_thunk() noexcept(false)
		{
			set_call_target(false);
		}

	public:
		Func get_thunked_function() const noexcept
		{
			return reinterpret_cast<Func>(static_cast<const Derived*>(this));
		}

		void* operator new(std::size_t size)
		{
			if constexpr (is_aligned_heapalloc_v<alignof(Derived)>)
			{
				return executable_alloc(size);
			}
			else
			{
				return aligned_executable_alloc<alignof(Derived)>(size);
			}
		}

		void operator delete(void* ptr) noexcept(false)
		{
			if constexpr (is_aligned_heapalloc_v<alignof(Derived)>)
			{
				return executable_free(ptr);
			}
			else
			{
				return aligned_executable_free(ptr);
			}
		}
	};
}