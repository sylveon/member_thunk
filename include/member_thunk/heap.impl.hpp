#pragma once
#include "./heap.hpp"
#include <mutex>
#include <sysinfoapi.h>

#include "./details/heap/region.hpp"
#include "./error/heap_not_empty.hpp"
#include "./error/invalid_memory_layout.hpp"
#include "./page.hpp"

namespace member_thunk
{
	template<typename T>
	void heap<T>::update_region(details::region<T>* region, bool was_full)
	{
		auto& current_list = was_full ? full_regions : used_regions;
		auto& new_list = was_full ? used_regions : free_regions;

		std::scoped_lock guard(lock);
		auto it = current_list.before_begin();
		while (true)
		{
			const auto before = it++;
			if (it == current_list.end())
			{
				break;
			}
			else if (&*it == region)
			{
				new_list.splice_after(new_list.before_begin(), current_list, before);
				return;
			}
		}
	}

	template<typename T>
	heap<T>::heap()
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		page_size = info.dwPageSize;
		allocation_granularity = info.dwAllocationGranularity;

		if (allocation_granularity % page_size != 0)
		{
			throw invalid_memory_layout();
		}
	}

	template<typename T>
	page<T> heap<T>::allocate_page()
	{
		std::scoped_lock guard(lock);
		if (!used_regions.empty())
		{
			auto& region = used_regions.front();
			std::unique_lock region_guard(region.lock);

			auto page = region.commit_page();
			if (region.full())
			{
				// the heap lock allows the region to be unlocked while spliced - if it tries to call update_region
				// while being spliced, it will block until we return from this method.
				region_guard.unlock();
				full_regions.splice_after(full_regions.before_begin(), used_regions, used_regions.before_begin());
			}

			return page;
		}
		else
		{
			if (free_regions.empty())
			{
				free_regions.emplace_front(this);
			}

			// no need to lock the region here, there are no pages that might attempt to free.
			auto page = free_regions.front().commit_page();
			used_regions.splice_after(used_regions.before_begin(), free_regions, free_regions.before_begin());

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
