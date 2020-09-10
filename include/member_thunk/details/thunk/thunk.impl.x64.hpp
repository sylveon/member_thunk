#pragma once
#include "./thunk.hpp"
#include <algorithm>
#include <cstring>

namespace member_thunk::details
{
	thunk::thunk(void* that, void* func) noexcept :
		mov_rax { 0x48, 0xB8 },
		function(func),
		mov_rcx { 0x48, 0xB9 },
		that(that),
		rex_jmp_rax { 0x48, 0xFF, 0xE0 }
	{
		static_assert(sizeof(thunk) == 32);

		std::memset(int_3, 0xCC, sizeof(int_3));
	}

	thunk::~thunk() noexcept
	{
		// TODO: uncomment when https://github.com/microsoft/STL/wiki/Changelog#expected-in-vs-2019-168-preview-3 ships
#pragma warning(suppress: 4244)
		std::fill(reinterpret_cast<volatile std::uint8_t*>(this), reinterpret_cast<volatile std::uint8_t*>(this + 1), /*static_cast<std::uint8_t>*/(0xCC));
	}
}