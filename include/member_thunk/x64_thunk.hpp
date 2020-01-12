#pragma once
#include "crtp_thunk.hpp"
#include <cstdint>

#include "common.hpp"

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// mov rcx, {this}
	// mov rax, {function}
	// rex_jmp rax
	// int 3
	// int 3
	// int 3
	// int 3
	// int 3
	// int 3
	// int 3
	// int 3
	// int 3
#ifdef __cpp_concepts  // MIGRATION: IDE concept support
	template<typename Func>
#else
	template<typename Func, typename>
#endif
	class thunk final : public crtp_thunk<thunk<Func>, Func>
	{
		std::uint8_t mov_rcx[2];
		void* that;
		std::uint8_t mov_rax[2];
		void* function;
		std::uint8_t rex_jmp_rax[3];
		std::uint8_t int_3[9];

	public:
		thunk(void* pThis, void* pFunc) :
			mov_rcx { 0x48, 0xB9 },
			that(pThis),
			mov_rax { 0x48, 0xB8 },
			function(pFunc),
			rex_jmp_rax { 0x48, 0xFF, 0xE0 },
			int_3 { 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC }
		{
			MEMBER_THUNK_STATIC_ASSERT_ALIGNOF_THIS();
			MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(32);

			this->init_thunk();
		}
	};

	MEMBER_THUNK_UNPACK
}