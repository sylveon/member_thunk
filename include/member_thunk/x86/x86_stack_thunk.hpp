#pragma once
#include "../base_thunk.hpp"
#include <cstdint>

#include "calling_convention_traits.hpp"
#include "../common.hpp"

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// push ebp
	// mov ebp, esp
	// mov eax, {function}
	// mov DWORD PTR [ebp+8], {this}
	// pop ebp
	// jmp eax
	template<typename Func>
		requires is_this_on_stack_v<Func>
	class thunk<Func, architecture::x86> final : public crtp_thunk<thunk<Func, architecture::x86>, Func>
	{
		std::uint8_t push_ebp;
		std::uint8_t mov_ebp_esp[2];
		std::uint8_t mov_eax;
		void* function;
		std::uint8_t mov_ebp_plus_8[3];
		void* that;
		std::uint8_t pop_ebp;
		std::uint8_t jmp_eax[2];

	public:
		template<typename Class, typename MemberFunc>
		thunk(Class* pThis, MemberFunc pFunc) :
			push_ebp(0x55),
			mov_ebp_esp { 0x8b, 0xEC },
			mov_eax(0xB8),
			function(reinterpret_cast<void*&>(pFunc)),
			mov_ebp_plus_8 { 0xC7, 0x45, 0x08 },
			that(pThis),
			pop_ebp(0x5d),
			jmp_eax { 0xFF, 0xE0 }
		{
			MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(18);

			this->flush();
		}
	};

	MEMBER_THUNK_UNPACK
}