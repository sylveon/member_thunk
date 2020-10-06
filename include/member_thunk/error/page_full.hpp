#pragma once
#include "./exception.hpp"

namespace member_thunk
{
	class page_full final : public exception
	{
	public:
		constexpr page_full() noexcept : exception("New thunks cannot be added, this page is full.") { }
	};
}
