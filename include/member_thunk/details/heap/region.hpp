#pragma once
#include <bitset>
#include <cstddef>

namespace member_thunk
{
	template<typename T>
	class heap;

	template<typename T>
	class page;
}

namespace member_thunk::details
{
	template<typename T>
	class region final
	{
		friend heap<T>;
		friend page<T>;

		heap<T>* parent;
		std::byte* base;
		std::bitset<16> page_availability;
		[[no_unique_address]] T lock;

		region(const region&) = delete;
		region& operator=(const region&) = delete;

		bool full() const noexcept;
		std::size_t find_free_page() const;
		page<T> commit_page();
		void mark_decommited(std::byte* page);

	public:
		region(heap<T>* parent);
		~region() noexcept(false);
	};
}
