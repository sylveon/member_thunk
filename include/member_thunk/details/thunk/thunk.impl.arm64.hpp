#pragma once
#include "./thunk.hpp"
#include <bit>
#include <cstring>

namespace member_thunk::details
{
	inline thunk::thunk(void* that, void* func) noexcept :
		ldr_x8_16 { 0x88, 0x00, 0x00, 0x58 },
		ldr_x0_20 { 0xA0, 0x00, 0x00, 0x58 },
		br_x8 { 0x00, 0x01, 0x1F, 0xD6 },
		function(func),
		that(that)
	{
		static_assert(sizeof(thunk) == 32);
		static_assert(sizeof(brk_F000) == sizeof(BREAK));

		std::memcpy(&brk_F000, &BREAK, sizeof(brk_F000));
	}

	inline thunk::~thunk() noexcept
	{
		fill(std::bit_cast<std::uint32_t>(BREAK));
	}
}
