#pragma once
#include "./region.hpp"
#include <bit>
#include <mutex>
#include <winapifamily.h>

#include "../../error/region_full.hpp"
#include "../../error/region_not_empty.hpp"
#include "../../heap.hpp"
#include "../../page.hpp"
#include "./memory.hpp"

namespace member_thunk::details
{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
	static constexpr std::uint32_t default_region_protection = PAGE_EXECUTE_READ | PAGE_TARGETS_INVALID;
#else
	static constexpr std::uint32_t default_region_protection = PAGE_READWRITE;
#endif

	template<typename T>
	bool region<T>::full() const noexcept
	{
		// guard against the Windows max macro :(
		return page_availability == (std::numeric_limits<page_availability_t>::max)();
	}

	template<typename T>
	int region<T>::find_free_page() const
	{
		const auto count = std::countr_one(page_availability);

		if (count < pages_per_region)
		{
			// regions are zero-indexed
			return pages_per_region - 1 - count;
		}
		else
		{
			throw region_full();
		}
	}

	template<typename T>
	page<T> region<T>::commit_page()
	{
		// implied lock from heap
		auto page_index = find_free_page();
		page new_page(this, base + (page_index * parent->layout.page_size));

		set_page_status(page_index, true);
		return new_page;
	}

	template<typename T>
	std::uint32_t region<T>::page_size() noexcept
	{
		return parent->layout.page_size;
	}

	template<typename T>
	void region<T>::mark_decommited(std::byte* page)
	{
		std::scoped_lock guard(lock);
		bool was_full = full();
		set_page_status(static_cast<int>((page - base) / parent->layout.page_size), false);

		if (was_full || page_availability == 0)
		{
			parent->update_region(this, was_full);
		}
	}

	template<typename T>
	void region<T>::set_page_status(int index, bool status) noexcept
	{
		int shift = pages_per_region - 1 - index;
		page_availability = (page_availability & ~(1 << shift)) | (status << shift);
	}

	template<typename T>
	region<T>::region(heap<T>* parent) :
		parent(parent),
		base(
			static_cast<std::byte*>(virtual_alloc(nullptr, parent->layout.allocation_granularity, MEM_RESERVE, default_region_protection))),
		page_availability(0)
	{ }

	template<typename T>
	region<T>::~region() noexcept(false)
	{
		if (page_availability != 0)
		{
			throw region_not_empty();
		}

		virtual_free(base, 0, MEM_RELEASE);
	}
}
