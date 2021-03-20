#pragma once
#include <minwindef.h>

#include "./default_heap.hpp"
#include "./details/heap/region.hpp"
#include "./details/list.hpp"

namespace member_thunk
{
	template<typename T>
	class page;

	template<typename T = details::default_lock_t>
	class heap final
	{
		friend page<T>;
		friend details::region<T>;

		// we use a linked list to obtain pointer stability, and remove exception failure points.
		details::list<details::region<T>> full_regions, used_regions, free_regions;
		DWORD page_size, allocation_granularity; // TODO: make these statics
		[[no_unique_address]] T lock;

		void update_region(details::region<T>* region, bool was_full);

	public:
		heap(const heap&) = delete;
		heap& operator=(const heap&) = delete;

		heap();
		page<T> allocate_page();
		void compact();
		~heap() noexcept(false);
	};
}
