#pragma once
#include "crtp_thunk.hpp"
#include <cstdint>

#include "architecture_traits.hpp"
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
#ifdef __cpp_concepts // MIGRATION: IDE concept support
	template<typename Func>
		requires is_architecture_v<architecture::arm64>
	class thunk<Func> final : public crtp_thunk<thunk<Func>, Func>
#else
	template<typename Func>
	class thunk<Func, std::enable_if_t<is_architecture_v<architecture::arm64>>> final : public crtp_thunk<thunk<Func>, Func>
#endif
	{
		std::uint8_t ldr_x0_16[4];
		std::uint8_t ldr_x8_20[4];
		std::uint8_t br_x8[4];
		std::uint8_t brk_F000[4];
		void* that;
		void* function;

	public:
		template<typename Class, typename MemberFunc>
		thunk(Class* pThis, MemberFunc pFunc) :
			ldr_x0_16 { 0x80, 0x00, 0x00, 0x58 },
			ldr_x8_20 { 0xA8, 0x00, 0x00, 0x58 },
			br_x8 { 0x00, 0x01, 0x1F, 0xD6 },
			brk_F000 { 0x00, 0x00, 0x3E, 0xD4 },
			that(pThis),
			function(reinterpret_cast<void*&>(pFunc))
		{
			MEMBER_THUNK_STATIC_ASSERT_ALIGNOF_THIS();
			MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(32);

			this->flush();
		}
	};

	MEMBER_THUNK_UNPACK
}