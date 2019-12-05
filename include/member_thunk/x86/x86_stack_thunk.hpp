#pragma once
#include "../base_thunk.hpp"
#include <cstdint>

#include "calling_convention_traits.hpp"
#include "../common.hpp"

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// mov eax, {function}
	// mov dword ptr [esp+4], {this}
	// jmp eax
	template<typename Func>
		requires is_this_on_stack_v<Func>
	class thunk<Func, architecture::x86> final : public crtp_thunk<thunk<Func, architecture::x86>, Func>
	{
		std::uint8_t mov_eax;
		void* function;
		std::uint8_t mov_esp_plus_4[4];
		void* that;
		std::uint8_t jmp_eax[2];

	public:
		template<typename Class, typename MemberFunc>
		thunk(Class* pThis, MemberFunc pFunc) :
			mov_eax(0xB8),
			function(reinterpret_cast<void*&>(pFunc)),
			mov_esp_plus_4 { 0xC7, 0x44, 0x24, 0x04 },
			that(pThis),
			jmp_eax { 0xFF, 0xE0 }
		{
			MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(15);

			this->flush();
		}
	};

	MEMBER_THUNK_UNPACK
}