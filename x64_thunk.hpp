#pragma once
#include "base_thunk.hpp"
#include <cstdint>

#include "common.hpp"
#include "function_traits.hpp"

namespace member_thunk
{
	MEMBER_THUNK_PACK

	// mov rcx, {this}
	// mov rax, {function}
	// rex_jmp rax
	template<typename T>
		MEMBER_THUNK_REQUIRES(is_function_pointer_with_pointer_sized_first_arg_v<T>)
	class x64_thunk final : public base_thunk
	{
		std::uint8_t mov_rcx[2];
		void* that;
		std::uint8_t mov_rax[2];
		void* function;
		std::uint8_t rex_jmp[3];

	public:
		template<typename U, typename V>
			MEMBER_THUNK_REQUIRES((is_class_member_function_pointer_with_same_args_except_first_v<T, U, V>))
		x64_thunk(U* pThis, V pFunc) :
			mov_rcx { 0x48, 0xB9 },
			that(pThis),
			mov_rax { 0x48, 0xB8 },
			function(reinterpret_cast<void*&>(pFunc)),
			rex_jmp { 0x48, 0xFF, 0xE0 }
		{
			MEMBER_THUNK_ASSERT_SIZE(x64_thunk, 23);

			flush(this, sizeof(*this));
		}

		T get_thunked_function() const
		{
			return reinterpret_cast<T>(this);
		}
	};

	MEMBER_THUNK_UNPACK
}