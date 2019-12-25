#pragma once
#include <errhandlingapi.h>
#include <heapapi.h>
#include <memory>
#include <new>
#include <processthreadsapi.h>
#include <stdexcept>
#include <system_error>
#include <wil/resource.h>
#include <winnt.h>

#define MEMBER_THUNK_PACK __pragma(pack(push, 1))
#define MEMBER_THUNK_UNPACK __pragma(pack(pop))

#define MEMBER_THUNK_STRINGIFY_INNER(s) #s
#define MEMBER_THUNK_STRINGIFY(s) MEMBER_THUNK_STRINGIFY_INNER(s)

#define MEMBER_THUNK_BASE_THUNK_ALIGNMENT 16

#define MEMBER_THUNK_STATIC_ASSERT_ALIGNOF_THIS() \
	static_assert( \
		alignof(decltype(*this)) == alignof(base_thunk), \
		"Thunk class should be aligned on a " \
			MEMBER_THUNK_STRINGIFY(MEMBER_THUNK_BASE_THUNK_ALIGNMENT) " bytes boundary" \
	)
#define MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(s) \
	static_assert(sizeof(*this) == s, "Thunk class should be have a size of " #s " bytes")

namespace member_thunk
{
	class alignas(MEMBER_THUNK_BASE_THUNK_ALIGNMENT) base_thunk
	{
		using offset_t = unsigned char;

		static constexpr bool is_aligned_heapalloc = MEMORY_ALLOCATION_ALIGNMENT >= MEMBER_THUNK_BASE_THUNK_ALIGNMENT;
		inline static wil::unique_hheap thunk_heap;

		[[noreturn]] static void throw_win32_error(DWORD error, const char* message)
		{
			throw std::system_error(static_cast<int>(error), std::system_category(), message);
		}

		[[noreturn]] static void throw_last_error(const char* message)
		{
			throw_win32_error(GetLastError(), message);
		}

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

		// Disable copy and move for all implementations
		base_thunk(const base_thunk&) = delete;
		base_thunk& operator=(const base_thunk&) = delete;

	protected:
		base_thunk() = default;

		static void flush(const void* ptr, std::size_t size)
		{
			if (!FlushInstructionCache(GetCurrentProcess(), ptr, size))
			{
				throw_last_error("FlushInstructionCache failed");
			}
		}

	public:
		void* operator new(std::size_t size)
		{
			std::size_t allocated_size = size;
			if constexpr (!is_aligned_heapalloc)
			{
				allocated_size += alignof(base_thunk);
			}

			if (void* ptr = HeapAlloc(init_heap(), 0, allocated_size))
			{
				if constexpr (!is_aligned_heapalloc)
				{
					const auto byte_ptr = reinterpret_cast<offset_t*>(ptr);

					// Reserve some space to store the offset.
					std::size_t usable_space = allocated_size - sizeof(offset_t);
					ptr = byte_ptr + sizeof(offset_t);

					if (std::align(alignof(base_thunk), size, ptr, usable_space))
					{
						const auto aligned_byte_ptr = reinterpret_cast<offset_t*>(ptr);
						aligned_byte_ptr[-1] = static_cast<offset_t>(aligned_byte_ptr - byte_ptr);
					}
					else
					{
						throw std::range_error("Failed to align pointer in allocated memory block");
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

		void operator delete(void* ptr, std::size_t size) noexcept(false)
		{
			if (ptr)
			{
				const auto byte_ptr = reinterpret_cast<offset_t*>(ptr);

				offset_t offset = 0;
				if constexpr (!is_aligned_heapalloc)
				{
					offset = byte_ptr[-1];
				}

				if (!HeapFree(init_heap(), 0, byte_ptr - offset))
				{
					throw_last_error("HeapFree failed");
				}
			}
		}
	};

	template<typename Derived, typename Func>
	class crtp_thunk : public base_thunk
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