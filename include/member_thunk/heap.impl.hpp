#pragma once
#include "./heap.hpp"
#include <mutex>

#include "./error/heap_not_empty.hpp"
#include "./page.hpp"

namespace member_thunk
{
	template<typename T>
	void heap<T>::update_region(details::region<T>* region, bool was_full)
	{
		using node_t = typename details::list<details::region<T>>::node;

		static_assert(
			std::is_standard_layout_v<node_t>, "node needs to be standard layout for casting from T* to node* to be well-defined");

		static_assert(offsetof(node_t, item) == 0, "item needs to be the first member for casting from T* to node* to be well-defined");

		auto& source = was_full ? full_regions : used_regions;
		auto& dest = was_full ? used_regions : free_regions;

		std::scoped_lock guard(lock);
		dest.take_node(source, reinterpret_cast<node_t*>(region));
	}

	template<typename T>
	heap<T>::heap() : layout(details::memory_layout::get_for_current_system())
	{ }

	template<typename T>
	page<T> heap<T>::allocate_page()
	{
		std::scoped_lock guard(lock);
		if (!used_regions.empty())
		{
			auto& region = used_regions.head()->item;
			std::unique_lock region_guard(region.lock);

			auto page = region.commit_page();
			if (region.full())
			{
				// the heap lock allows the region to be unlocked while spliced - if it tries to call update_region
				// while being spliced, it will block until we return from this method.
				region_guard.unlock();
				full_regions.take_head(used_regions);
			}

			return page;
		}
		else
		{
			if (free_regions.empty())
			{
				free_regions.emplace_head(this);
			}

			// no need to lock the region here, there are no pages that might attempt to free.
			auto page = free_regions.head()->item.commit_page();
			used_regions.take_head(free_regions);

			return page;
		}
	}

	template<typename T>
	void heap<T>::compact()
	{
		std::scoped_lock guard(lock);
		free_regions.clear();
	}

	template<typename T>
	heap<T>::~heap() noexcept(false)
	{
		if (!full_regions.empty() || !used_regions.empty())
		{
			throw heap_not_empty();
		}
	}
}
