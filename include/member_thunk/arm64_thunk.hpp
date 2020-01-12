#pragma once
#include "crtp_thunk.hpp"
#include <cstdint>

#include "common.hpp"

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// ldr x0, #16
	// ldr x8, #20
	// br x8
	// brk #0xF000
	// {this}
	// {function}
	template<typename Func>
	class thunk<Func> final : public crtp_thunk<thunk<Func>, Func>
	{
		std::uint8_t ldr_x0_16[4];
		std::uint8_t ldr_x8_20[4];
		std::uint8_t br_x8[4];
		std::uint8_t brk_F000[4];
		void* that;
		void* function;

	public:
		thunk(void* pThis, void* pFunc) :
			ldr_x0_16 { 0x80, 0x00, 0x00, 0x58 },
			ldr_x8_20 { 0xA8, 0x00, 0x00, 0x58 },
			br_x8 { 0x00, 0x01, 0x1F, 0xD6 },
			brk_F000 { 0x00, 0x00, 0x3E, 0xD4 },
			that(pThis),
			function(pFunc)
		{
			MEMBER_THUNK_STATIC_ASSERT_ALIGNOF_THIS();
			MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(32);

			this->init_thunk();
		}
	};

	MEMBER_THUNK_UNPACK
}