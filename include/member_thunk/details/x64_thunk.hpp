#pragma once
#include "base_thunk.hpp"
#include <cstdint>

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// mov rax, {function}
	// mov rcx, {this}
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
	template<typename Func>
	class thunk final : public details::base_thunk<thunk<Func>, Func>
	{
		std::uint8_t mov_rax[2];
		void* function;
		std::uint8_t mov_rcx[2];
		void* that;
		std::uint8_t rex_jmp_rax[3];
		std::uint8_t int_3[9];

	public:
		thunk(void* pThis, void* pFunc) :
			mov_rax { 0x48, 0xB8 },
			function(pFunc),
			mov_rcx { 0x48, 0xB9 },
			that(pThis),
			rex_jmp_rax { 0x48, 0xFF, 0xE0 },
			int_3 { 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC }
		{
			this->init_thunk<32>();
		}
	};

	MEMBER_THUNK_UNPACK
}