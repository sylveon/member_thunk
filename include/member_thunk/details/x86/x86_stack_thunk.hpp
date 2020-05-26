#pragma once
#include "../base_thunk.hpp"
#include <cstdint>

#include "calling_convention_traits.hpp"
#include "../../common.hpp"

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// mov eax, {function}
	// mov dword ptr [esp+4], {this}
	// jmp eax
	// int 3
	template<typename Func>
	requires details::is_this_on_stack_v<Func>
	class thunk final : public details::base_thunk<thunk<Func>, Func>
	{
		std::uint8_t mov_eax;
		void* function;
		std::uint8_t mov_esp_plus_4[4];
		void* that;
		std::uint8_t jmp_eax[2];
		std::uint8_t int_3;

	public:
		thunk(void* pThis, void* pFunc) :
			mov_eax(0xB8),
			function(pFunc),
			mov_esp_plus_4 { 0xC7, 0x44, 0x24, 0x04 },
			that(pThis),
			jmp_eax { 0xFF, 0xE0 },
			int_3(0xCC)
		{
			this->init_thunk<16>();
		}
	};

	MEMBER_THUNK_UNPACK
}