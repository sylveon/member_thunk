#pragma once
#include "../crtp_thunk.hpp"
#include <cstdint>

#include "calling_convention_traits.hpp"
#include "../common.hpp"

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// mov eax, {function}
	// mov dword ptr [esp+4], {this}
	// jmp eax
	// int 3
#ifdef __cpp_concepts // MIGRATION: IDE concept support
	template<typename Func>
		requires is_this_on_stack_v<Func>
	class thunk<Func> final : public crtp_thunk<thunk<Func>, Func>
#else
	template<typename Func>
	class thunk<Func, std::enable_if_t<is_this_on_stack_v<Func>>> final : public crtp_thunk<thunk<Func>, Func>
#endif
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
			MEMBER_THUNK_STATIC_ASSERT_ALIGNOF_THIS();
			MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(16);

			this->init_thunk();
		}
	};

	MEMBER_THUNK_UNPACK
}