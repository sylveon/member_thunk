#pragma once
#include <string_view>

namespace member_thunk
{
	class exception
	{
	protected:
		std::string_view what_str;

		constexpr exception(std::string_view msg) noexcept : what_str(msg) { }

	public:
		constexpr std::string_view what() const noexcept
		{
			return what_str;
		}

		virtual ~exception() = default;
	};
}
