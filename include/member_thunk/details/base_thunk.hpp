#pragma once
#include <concepts>
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
		requires (sizeof(Derived) == size) && (alignof(Derived) <= alignof(base_thunk))
		void init_thunk()
		{
			flush();
			set_call_target(true);
		}

		// This is sinful
#ifdef __cpp_lib_concepts // MIGRATION: IDE concept support
		template<std::unsigned_integral T>
#else
		template<typename T>
#endif
		requires (sizeof(Derived) % sizeof(T) == 0)
		void fill(T val) noexcept
		{
			const auto that = static_cast<Derived*>(this);

			auto ptr = reinterpret_cast<volatile T*>(that);
			const auto end = reinterpret_cast<volatile T*>(that + 1);

			for (; ptr < end; ++ptr)
			{
				*ptr = val;
			}
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

		void operator delete(base_thunk* ptr, std::destroying_delete_t) noexcept(false)
		{
			const auto that = static_cast<Derived*>(ptr);
			that->set_call_target(false);
			that->clear();
			that->flush();
			that->~Derived();

			if constexpr (is_aligned_heapalloc_v<alignof(Derived)>)
			{
				return executable_free(that);
			}
			else
			{
				return aligned_executable_free(that);
			}
		}
	};
}