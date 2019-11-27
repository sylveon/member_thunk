#pragma once
#include "base_thunk.hpp"
#include <cstdint>

#include "common.hpp"

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// mov rcx, {this}
	// mov rax, {function}
	// rex_jmp rax
	template<typename Func>
	class thunk<Func, architecture::x64> final : public crtp_base_thunk<thunk<Func, architecture::x64>, Func>
	{
		std::uint8_t mov_rcx[2];
		void* that;
		std::uint8_t mov_rax[2];
		void* function;
		std::uint8_t rex_jmp_rax[3];

	public:
		template<typename Class, typename MemberFunc>
		thunk(Class* pThis, MemberFunc pFunc) :
			mov_rcx { 0x48, 0xB9 },
			that(pThis),
			mov_rax { 0x48, 0xB8 },
			function(reinterpret_cast<void*&>(pFunc)),
			rex_jmp_rax { 0x48, 0xFF, 0xE0 }
		{
			MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(23);

			this->flush();
		}
	};

	MEMBER_THUNK_UNPACK
}