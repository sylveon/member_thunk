#pragma once
#include "./exception.hpp"
#include <errhandlingapi.h>
#include <string_view>
#include <minwindef.h>

namespace member_thunk
{
	class win32_error final : public exception
	{
		std::string_view api;
		DWORD error;

	public:
		constexpr win32_error(std::string_view api, DWORD error) noexcept :
			exception("A Windows API call failed."),
			api(api),
			error(error)
		{ }

		win32_error(std::string_view api) noexcept : win32_error(api, GetLastError()) { }

		constexpr std::string_view failing_api() const noexcept
		{
			return api;
		}

		constexpr DWORD error_code() const noexcept
		{
			return error;
		}
	};
}