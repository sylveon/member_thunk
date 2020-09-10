#pragma once
#include "./exception.hpp"

namespace member_thunk
{
	class invalid_memory_layout final : public exception
	{
	public:
		constexpr invalid_memory_layout() noexcept :
			exception("member_thunk is not compatible with this system's memory layout.")
		{ }
	};
}
