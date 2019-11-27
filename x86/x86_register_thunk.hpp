#pragma once
#include "../base_thunk.hpp"
#include "x86_thunk.hpp"
#include <cstdint>

#include "calling_convention_traits.hpp"
#include "../common.hpp"

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// mov eax, {function}
	// mov ecx, {this}
	// jmp eax
	template<typename Func>
	class x86_thunk<Func, std::enable_if_t<is_this_on_register_v<Func>>> final : public base_thunk
	{
		std::uint8_t mov_eax;
		void* function;
		std::uint8_t mov_ecx;
		void* that;
		std::uint8_t jmp_eax[2];

	public:
		template<typename Class, typename MemberFunc>
		x86_thunk(Class* pThis, MemberFunc pFunc) :
			mov_eax(0xB8),
			function(reinterpret_cast<void*&>(pFunc)),
			mov_ecx(0xB9),
			that(pThis),
			jmp_eax { 0xFF, 0xE0 }
		{
			MEMBER_THUNK_ASSERT_SIZE(x86_thunk, 12);
			MEMBER_THUNK_ASSERT_ARCHITECTURE(x86);

			flush(this, sizeof(*this));
		}

		Func get_thunked_function() const
		{
			return reinterpret_cast<Func>(this);
		}
	};

	MEMBER_THUNK_UNPACK
}