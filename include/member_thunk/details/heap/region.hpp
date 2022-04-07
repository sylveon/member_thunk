#pragma once
#include <cstddef>
#include <cstdint>
#include <limits>

#include "./abstract_region.hpp"

namespace member_thunk
{
	template<typename T>
	class heap;

	class page;
}

namespace member_thunk::details
{
	using page_availability_t = std::uint16_t;
	static constexpr int pages_per_region = std::numeric_limits<page_availability_t>::digits;

	template<typename T>
	class region final : abstract_region
	{
		friend heap<T>;

		heap<T>* parent;
		std::byte* base;
		page_availability_t page_availability;
		[[no_unique_address]] T lock;

		region(const region&) = delete;
		region& operator=(const region&) = delete;

		bool full() const noexcept;
		int find_free_page() const;
		page commit_page();
		std::uint32_t page_size() noexcept override;
		void mark_decommited(std::byte* page) override;
		void set_page_status(int index, bool status) noexcept;

	public:
		region(heap<T>* parent);
		~region() noexcept(false);
	};
}
