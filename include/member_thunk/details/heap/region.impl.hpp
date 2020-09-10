#pragma once
#include "./region.hpp"
#include <mutex>

#include "../../error/invalid_memory_layout.hpp"
#include "../../error/region_full.hpp"
#include "../../error/region_not_empty.hpp"
#include "../../heap.hpp"
#include "../../page.hpp"
#include "./memory.hpp"

namespace member_thunk::details
{
	template<typename T>
	bool region<T>::full() const noexcept
	{
		return page_availability.all();
	}

	template<typename T>
	std::size_t region<T>::find_free_page() const
	{
		// std::bitset doesn't have iterators reeeeeeeeee
		for (std::size_t i = 0; i < page_availability.size(); ++i)
		{
			if (!page_availability.test(i))
			{
				return i;
			}
		}

		throw region_full();
	}

	template<typename T>
	page<T> region<T>::commit_page()
	{
		// implied lock from heap
		auto page_index = find_free_page();
		page new_page(this, base + (page_index * parent->page_size));

		page_availability.set(page_index);
		return new_page;
	}

	template<typename T>
	void region<T>::mark_decommited(std::byte* page)
	{
		std::scoped_lock guard(lock);
		bool was_full = full();
		page_availability.set((page - base) / parent->page_size, false);

		if (was_full || page_availability.none())
		{
			parent->update_region(this, was_full);
		}
	}

	template<typename T>
	region<T>::region(heap<T>* parent) :
		parent(parent),
		base(static_cast<std::byte*>(virtual_alloc(nullptr, parent->allocation_granularity, MEM_RESERVE, PAGE_EXECUTE_READ | PAGE_TARGETS_INVALID)))
	{
		if (page_availability.size() != parent->allocation_granularity / parent->page_size)
		{
			throw invalid_memory_layout();
		}
	}

	template<typename T>
	region<T>::~region() noexcept(false)
	{
		if (page_availability.any())
		{
			throw region_not_empty();
		}

		virtual_free(base, 0, MEM_RELEASE);
	}
}
