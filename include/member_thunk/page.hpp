#pragma once
#include <cstddef>
#include <minwindef.h>
#include <utility>

#include "./default_heap.hpp"
#include "./details/function_traits.hpp"

namespace member_thunk
{
	namespace details
	{
		template<typename T>
		class region;

		class thunk;
	}

	template<typename T = details::default_lock_t>
	class page final
	{
		friend details::region<T>;

		bool executable; // whether the page has been marked executable
		DWORD size; // saved to skip double indirection, does not affect class size because there would be padding otherwise
		details::region<T>* parent; // the region this page is from
		details::thunk* begin; // the beginning of the page
		details::thunk* end; // pointer past the end of currently created thunks

		page(details::region<T>* parent, std::byte* address);

		void set_call_target(bool valid);
		void free();
		details::thunk* new_thunk(void* that, void* func);
		details::thunk* page_end() const noexcept;
		std::size_t byte_offset(details::thunk* thunk) const noexcept;

	public:
		page(const page&) = delete;
		page& operator=(const page&) = delete;

		constexpr page(page&& other) noexcept :
			executable(other.executable),
			size(other.size),
			parent(other.parent),
			begin(std::exchange(other.begin, nullptr)),
			end(other.end)
		{ }

		constexpr page& operator=(page&& other) noexcept
		{
			if (this != &other)
			{
				std::swap(executable, other.executable);
				std::swap(size, other.size);
				std::swap(parent, other.parent);
				std::swap(begin, other.begin);
				std::swap(end, other.end);
			}

			return *this;
		}

		template<typename Func, typename MemberFunc>
		// clang-format off
		requires details::is_compatible_function_types_v<Func, MemberFunc>
		Func make_thunk(details::get_this_ptr_t<MemberFunc> that, MemberFunc func)
		// clang-format on
		{
			// slightly cursed.
			return reinterpret_cast<Func>(new_thunk(const_cast<void*>(static_cast<const void*>(that)), reinterpret_cast<void*&>(func)));
		}

		bool full() const noexcept;
		void mark_executable();

		~page() noexcept(false)
		{
			if (begin)
			{
				free();
			}
		}
	};
}
