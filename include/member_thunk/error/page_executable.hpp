#pragma once
#include "./exception.hpp"

namespace member_thunk
{
	class page_executable final : public exception
	{
	public:
		constexpr page_executable() noexcept :
			exception("New thunks cannot be added, this page is already executable.")
		{ }
	};
}
