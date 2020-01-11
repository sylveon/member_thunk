#pragma once
#include <tuple>
#include <type_traits>

namespace member_thunk
{
	template<typename T>
	inline constexpr bool is_pointer_like_v = std::is_pointer_v<T> || (std::is_integral_v<T> && sizeof(T) == sizeof(void*));

	template<typename T>
	inline constexpr bool is_scalar_or_void_v = std::is_void_v<Ret> || std::is_scalar_v<Ret>;

	template<typename Func, typename Class, typename MemberFunc>
	inline constexpr bool is_compatible_function_types_v = false;

#define MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(convention, specifiers) \
	template<typename Ret, typename Class, typename FirstArg, typename... Args> \
	inline constexpr bool is_compatible_function_types_v< \
		Ret(convention*)(FirstArg, Args...), \
		Class, Ret(convention Class::*)(Args...) specifiers \
	> = is_pointer_like_v<FirstArg> && is_scalar_or_void_v<Ret>;

#define MEMBER_THUNK_GENERATE_NOEXCEPT_SPECIALIZATION(convention, specifiers) \
	MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(convention, specifiers) \
	MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(convention, specifiers noexcept)

#define MEMBER_THUNK_GENERATE_CONST_SPECIALIZATION(convention) \
	MEMBER_THUNK_GENERATE_NOEXCEPT_SPECIALIZATION(convention, ) \
	MEMBER_THUNK_GENERATE_NOEXCEPT_SPECIALIZATION(convention, const)

	// Specifying any calling convention on a function pointer in non-x86 will
	// explicitely specify the regular calling convention for that platform.
	// /Gv would make __vectorcall implicit on regular pointers in x64.
	MEMBER_THUNK_GENERATE_CONST_SPECIALIZATION(__stdcall)
#if defined(_M_AMD64) || defined(_M_IX86)
	// Same than normal x64 calling convention/x86 __fastcall but with vector
	// registers used to pass vector parameters.
	MEMBER_THUNK_GENERATE_CONST_SPECIALIZATION(__vectorcall)
#endif
#if defined(_M_IX86)
	MEMBER_THUNK_GENERATE_CONST_SPECIALIZATION(__cdecl)
	MEMBER_THUNK_GENERATE_CONST_SPECIALIZATION(__fastcall)
#endif

#undef MEMBER_THUNK_GENERATE_CONST_SPECIALIZATION
#undef MEMBER_THUNK_GENERATE_NOEXCEPT_SPECIALIZATION
#undef MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION
}