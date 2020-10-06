#pragma once
#include "./exception.hpp"

namespace member_thunk
{
	class region_full final : public exception
	{
	public:
		constexpr region_full() noexcept : exception("New pages cannot be created, this region is full.") { }
	};
}
