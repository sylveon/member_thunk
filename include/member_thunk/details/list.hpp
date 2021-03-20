#pragma once
#include <type_traits>
#include <utility>

namespace member_thunk::details
{
	template<typename T>
	class list
	{
	public:
		struct node
		{
			T item;
			node* previous;
			node* next;

			template<typename... Args>
			constexpr node(node* next, Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) :
				item(std::forward<Args>(args)...),
				previous(nullptr),
				next(next)
			{ }
		};

		constexpr list() noexcept : head_ptr(nullptr) { }
		constexpr list(list&& other) noexcept : head_ptr(std::exchange(other.head_ptr, nullptr)) { }

		constexpr list& operator=(list&& other) noexcept
		{
			if (this != &other)
			{
				std::swap(head_ptr, other.head_ptr);
			}
		}

		list(const list&) = delete;
		list& operator=(const list&) = delete;

		constexpr bool empty() const noexcept { return head_ptr == nullptr; }
		constexpr node* head() noexcept { return head_ptr; }

		template<typename... Args>
		void emplace_head(Args&&... args)
		{
			auto current = head_ptr;
			head_ptr = new node(current, std::forward<Args>(args)...);
			if (current)
			{
				current->previous = head_ptr;
			}
		}

		void take_head(list& other) noexcept;
		void take_node(list& other, node* node_ptr) noexcept;
		void pop_head() noexcept(std::is_nothrow_destructible_v<T>);
		void clear() noexcept(std::is_nothrow_destructible_v<T>);
		~list() noexcept(std::is_nothrow_destructible_v<T>);

	private:
		node* head_ptr;
	};
}
