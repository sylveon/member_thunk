#pragma once
#include <cstddef>
#include <cstdint>

namespace member_thunk::details
{
	struct memory_layout
	{
		std::uint32_t page_size, allocation_granularity;

		static memory_layout get_for_current_system();
	};

	void flush_instruction_cache(void* ptr, std::size_t size);
	void* virtual_alloc(void* address, std::size_t size, std::uint32_t type, std::uint32_t protection);
	std::uint32_t virtual_protect(void* ptr, std::size_t size, std::uint32_t protection);
	void virtual_free(void* ptr, std::size_t size, std::uint32_t free_type);
}
