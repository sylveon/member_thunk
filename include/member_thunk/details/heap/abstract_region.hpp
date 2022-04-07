#pragma once
#include <cstddef>
#include <cstdint>

namespace member_thunk::details
{
	class abstract_region
	{
    public:
        virtual std::uint32_t page_size() noexcept = 0;
        virtual void mark_decommited(std::byte* page) = 0;
	};
}
