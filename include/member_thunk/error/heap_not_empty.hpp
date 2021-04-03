#pragma once
#include "./exception.hpp"

namespace member_thunk
{
	class heap_not_empty final : public exception
	{
	public:
		constexpr heap_not_empty() noexcept : exception("This heap has been destroyed before being emptied.") { }
	};
}
