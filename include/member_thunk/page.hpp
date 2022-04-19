#pragma once
#include <cstddef>
#include <cstdint>
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

	class page final
	{
		template<typename T>
		friend class details::region;

		using free_callback_t = void (*)(std::byte* page, void* data);

		details::thunk* begin; // the beginning of the page
		details::thunk* end; // pointer past the end of currently created thunks
		free_callback_t callback; // called when the page is freed
		void* data; // data for the callback
		std::uint32_t size; // size of the page
		bool executable; // whether the page has been marked executable

		page(std::byte* address, std::uint32_t page_size, free_callback_t free_callback, void* callback_data);

		void set_call_target(bool valid);
		void free();
		details::thunk* new_thunk(void* that, void* func);
		details::thunk* page_end() const noexcept;
		std::size_t byte_offset(details::thunk* thunk) const noexcept;

	public:
		page(const page&) = delete;
		page& operator=(const page&) = delete;

		constexpr page(page&& other) noexcept :
			begin(std::exchange(other.begin, nullptr)),
			end(other.end),
			callback(other.callback),
			data(other.data),
			size(other.size),
			executable(other.executable)
		{ }

		constexpr page& operator=(page&& other) noexcept
		{
			if (this != &other)
			{
				std::swap(begin, other.begin);
				std::swap(end, other.end);
				std::swap(callback, other.callback);
				std::swap(data, other.data);
				std::swap(size, other.size);
				std::swap(executable, other.executable);
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
