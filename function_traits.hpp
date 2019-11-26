#pragma once
#include <tuple>
#include <type_traits>

namespace member_thunk
{
	template<typename Func>
	inline constexpr bool is_function_pointer_with_pointer_sized_first_arg_v = false;

	template<typename Func, typename Class, typename MemberFunc>
	inline constexpr bool is_class_member_function_pointer_with_same_args_except_first_v = false;

#define MEMBER_THUNK_GENERATE_BARE_SPECIALIZATION(convention) \
	template<typename Ret, typename FirstArg, typename... Args> \
	inline constexpr bool is_function_pointer_with_pointer_sized_first_arg_v<Ret(convention*)(FirstArg, Args...)> = sizeof(FirstArg) == sizeof(void*)

#define MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(convention, specifiers) \
	template<typename Ret, typename Class, typename FirstArg, typename... Args> \
	inline constexpr bool is_class_member_function_pointer_with_same_args_except_first_v<Ret(convention*)(FirstArg, Args...), Class, Ret(convention Class::*)(Args...) specifiers> = true

#define MEMBER_THUNK_GENERATE_CALLING_CONVENTION_SPECIALIZATION(convention) \
	MEMBER_THUNK_GENERATE_BARE_SPECIALIZATION(convention); \
	MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(convention, ); \
	MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(convention, const);

	// Specifying any calling convention on a function pointer in non-x86 will
	// explicitely specify the regular calling convention for that platform.
	// /Gv would make __vectorcall implicit on regular pointers in x64.
	MEMBER_THUNK_GENERATE_CALLING_CONVENTION_SPECIALIZATION(__stdcall)
#if defined(_M_AMD64) || defined(_M_IX86)
	// Same than normal x64 calling convention/x86 __fastcall but with vector
	// registers used to pass vector parameters.
	MEMBER_THUNK_GENERATE_CALLING_CONVENTION_SPECIALIZATION(__vectorcall)
#endif
#if defined(_M_IX86)
	MEMBER_THUNK_GENERATE_CALLING_CONVENTION_SPECIALIZATION(__cdecl)
	MEMBER_THUNK_GENERATE_CALLING_CONVENTION_SPECIALIZATION(__fastcall)
#endif

#undef MEMBER_THUNK_GENERATE_CALLING_CONVENTION_SPECIALIZATION
#undef MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION
#undef MEMBER_THUNK_GENERATE_BARE_SPECIALIZATION
}