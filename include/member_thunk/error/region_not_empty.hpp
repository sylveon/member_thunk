#pragma once
#include "./exception.hpp"

namespace member_thunk
{
	class region_not_empty final : public exception
	{
	public:
		constexpr region_not_empty() noexcept :
			exception("This region been destroyed before being emptied.")
		{ }
	};
}
