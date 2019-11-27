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
	class x64_thunk final : public base_thunk
	{
		std::uint8_t mov_rcx[2];
		void* that;
		std::uint8_t mov_rax[2];
		void* function;
		std::uint8_t rex_jmp_rax[3];

	public:
		template<typename Class, typename MemberFunc>
		x64_thunk(Class* pThis, MemberFunc pFunc) :
			mov_rcx { 0x48, 0xB9 },
			that(pThis),
			mov_rax { 0x48, 0xB8 },
			function(reinterpret_cast<void*&>(pFunc)),
			rex_jmp_rax { 0x48, 0xFF, 0xE0 }
		{
			MEMBER_THUNK_ASSERT_SIZE(x64_thunk, 23);
			MEMBER_THUNK_ASSERT_ARCHITECTURE(x64);

			flush(this, sizeof(*this));
		}

		Func get_thunked_function() const
		{
			return reinterpret_cast<Func>(this);
		}
	};

	MEMBER_THUNK_UNPACK
}