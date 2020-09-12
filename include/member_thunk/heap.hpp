#pragma once
#include <forward_list>
#include <minwindef.h>

#include "./details/common.hpp"

namespace member_thunk
{
	template<typename T>
	class page;

	namespace details
	{
		template<typename T>
		class region;
	}

	template<typename T = details::default_lock_t>
	class heap final
	{
		friend page<T>;
		friend details::region<T>;

		// we use a linked list to obtain pointer stability, and remove exception failure points.
		// because push_back followed by pop_back on a vector may throw, but splice_after does not.
		std::forward_list<details::region<T>> full_regions, used_regions, free_regions;
		DWORD page_size, allocation_granularity;
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
