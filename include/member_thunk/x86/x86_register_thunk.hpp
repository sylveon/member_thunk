#pragma once
#include "../crtp_thunk.hpp"
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
	// int 3
	// int 3
	// int 3
	// int 3
#ifdef __cpp_concepts // MIGRATION: IDE concept support
	template<typename Func>
		requires is_architecture_v<architecture::x86> && is_this_on_register_v<Func>
	class thunk<Func> final : public crtp_thunk<thunk<Func>, Func>
#else
	template<typename Func>
	class thunk<Func, std::enable_if_t<is_architecture_v<architecture::x86> && is_this_on_register_v<Func>>> final : public crtp_thunk<thunk<Func>, Func>
#endif
	{
		std::uint8_t mov_eax;
		void* function;
		std::uint8_t mov_ecx;
		void* that;
		std::uint8_t jmp_eax[2];
		std::uint8_t int_3[4];

	public:
		template<typename Class, typename MemberFunc>
		thunk(Class* pThis, MemberFunc pFunc) :
			mov_eax(0xB8),
			function(reinterpret_cast<void*&>(pFunc)),
			mov_ecx(0xB9),
			that(pThis),
			jmp_eax { 0xFF, 0xE0 },
			int_3 { 0xCC, 0xCC, 0xCC, 0xCC }
		{
			MEMBER_THUNK_STATIC_ASSERT_ALIGNOF_THIS();
			MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(16);

			this->flush();
		}
	};

	MEMBER_THUNK_UNPACK
}