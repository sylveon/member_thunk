#pragma once
#include "base_thunk.hpp"
#include <cstdint>

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// ldr x8, #16
	// ldr x0, #20
	// br x8
	// brk #0xF000
	// {function}
	// {this}
#ifdef __cpp_concepts  // MIGRATION: IDE concept support
	template<typename Func>
#else
	template<typename Func, typename>
#endif
	class thunk final : public details::base_thunk<thunk<Func>, Func>
	{
		std::uint8_t ldr_x8_16[4];
		std::uint8_t ldr_x0_20[4];
		std::uint8_t br_x8[4];
		std::uint8_t brk_F000[4];
		void* function;
		void* that;

	public:
		thunk(void* pThis, void* pFunc) :
			ldr_x8_16 { 0x88, 0x00, 0x00, 0x58 },
			ldr_x0_20 { 0xA0, 0x00, 0x00, 0x58 },
			br_x8 { 0x00, 0x01, 0x1F, 0xD6 },
			brk_F000 { 0x00, 0x00, 0x3E, 0xD4 },
			function(pFunc),
			that(pThis)
		{
			this->init_thunk<32>();
		}
	};

	MEMBER_THUNK_UNPACK
}