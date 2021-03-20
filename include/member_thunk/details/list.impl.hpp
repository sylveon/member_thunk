#pragma once
#include "./list.hpp"

namespace member_thunk::details
{
	template<typename T>
	void list<T>::take_head(list& other) noexcept
	{
		if (const auto other_head = other.head_ptr)
		{
			other.head_ptr = other_head->next;
			if (other.head_ptr)
			{
				other.head_ptr->previous = nullptr;
			}

			if (head_ptr)
			{
				head_ptr->previous = other_head;
			}

			other_head->next = head_ptr;
			head_ptr = other_head;
		}
	}

	template<typename T>
	void list<T>::take_node(list& other, node* node_ptr) noexcept
	{
		if (node_ptr != other.head_ptr)
		{
			node_ptr->previous->next = node_ptr->next;
			if (node_ptr->next)
			{
				node_ptr->next->previous = node_ptr->previous;
			}

			node_ptr->previous = nullptr;
			node_ptr->next = head_ptr;
			head_ptr = node_ptr;
		}
		else
		{
			take_head(other);
		}
	}

	template<typename T>
	void list<T>::pop_head() noexcept(std::is_nothrow_destructible_v<T>)
	{
		const auto new_head = head_ptr->next;
		delete head_ptr;

		// exception-safety: only set new head once previous head is completely deleted
		head_ptr = new_head;
	}

	template<typename T>
	void list<T>::clear() noexcept(std::is_nothrow_destructible_v<T>)
	{
		while (head_ptr != nullptr)
		{
			pop_head();
		}
	}

	template<typename T>
	list<T>::~list() noexcept(std::is_nothrow_destructible_v<T>)
	{
		clear();
	}
}
