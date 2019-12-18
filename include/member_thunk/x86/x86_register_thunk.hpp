#pragma once
#include "../base_thunk.hpp"
#include <cstdint>

#include "../architecture_traits.hpp"
#include "calling_convention_traits.hpp"
#include "../common.hpp"

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// mov eax, {function}
	// mov ecx, {this}
	// jmp eax
	template<typename Func>
		requires is_architecture_v<architecture::x86> && is_this_on_register_v<Func>
	class thunk<Func> final : public crtp_thunk<thunk<Func>, Func>
	{
		std::uint8_t mov_eax;
		void* function;
		std::uint8_t mov_ecx;
		void* that;
		std::uint8_t jmp_eax[2];
		std::uint8_t int3[4];

	public:
		template<typename Class, typename MemberFunc>
		thunk(Class* pThis, MemberFunc pFunc) :
			mov_eax(0xB8),
			function(reinterpret_cast<void*&>(pFunc)),
			mov_ecx(0xB9),
			that(pThis),
			jmp_eax { 0xFF, 0xE0 },
			int3 { 0xCC, 0xCC, 0xCC, 0xCC }
		{
			MEMBER_THUNK_STATIC_ASSERT_ALIGNOF_THIS(16);
			MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(16);

			this->flush();
		}
	};

	MEMBER_THUNK_UNPACK
}