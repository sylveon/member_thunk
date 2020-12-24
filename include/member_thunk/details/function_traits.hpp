#pragma once
#include <type_traits>

namespace member_thunk::details
{
	template<typename T>
	inline constexpr bool is_pointer_like_v = std::is_scalar_v<T> && !std::is_floating_point_v<T> && sizeof(T) == sizeof(void*);

	template<typename T>
	inline constexpr bool is_scalar_or_void_v = std::is_void_v<T> || std::is_scalar_v<T>;

	template<typename Func, typename MemberFunc>
	inline constexpr bool is_compatible_function_types_v = false;

	template<typename MemberFunc>
	struct get_this;

	template<typename MemberFunc>
	using get_this_ptr_t = std::add_pointer_t<typename get_this<MemberFunc>::type>;

	// clang-format off
#define MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(convention, specifiers, ptr_specifiers) \
	template<typename Ret, typename Class, typename FirstArg, typename... Args> \
	inline constexpr bool is_compatible_function_types_v< \
		Ret(convention*)(FirstArg, Args...) ptr_specifiers, \
		Ret(convention Class::*)(Args...) specifiers \
	> = is_pointer_like_v<FirstArg> && is_scalar_or_void_v<Ret>;

#define MEMBER_THUNK_GENERATE_THIS_SPECIALIZATION(convention, inheritance, specifiers) \
	template<typename Ret, typename Class, typename... Args> \
	struct get_this<Ret(convention Class::*)(Args...) specifiers> : inheritance<Class> { };
	// clang-format on

#define MEMBER_THUNK_GENERATE_NOEXCEPT_SPECIALIZATION(convention, inheritance, specifiers) \
	MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(convention, specifiers, ) \
	MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(convention, specifiers noexcept, ) \
	MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(convention, specifiers noexcept, noexcept) \
	MEMBER_THUNK_GENERATE_THIS_SPECIALIZATION(convention, inheritance, specifiers) \
	MEMBER_THUNK_GENERATE_THIS_SPECIALIZATION(convention, inheritance, specifiers noexcept)

#define MEMBER_THUNK_GENERATE_CONST_SPECIALIZATION(convention) \
	MEMBER_THUNK_GENERATE_NOEXCEPT_SPECIALIZATION(convention, std::type_identity, ) \
	MEMBER_THUNK_GENERATE_NOEXCEPT_SPECIALIZATION(convention, std::add_const, const)

	// Specifying any calling convention on a function pointer will
	// explicitely specify the regular calling convention for that platform.
	// The compiler option /Gv would make __vectorcall implicit on regular
	// pointers in x64.
	MEMBER_THUNK_GENERATE_CONST_SPECIALIZATION(__stdcall)
#if defined(_M_AMD64)
	// Same than normal x64 calling convention but with vector registers used
	// to pass vector parameters.
	MEMBER_THUNK_GENERATE_CONST_SPECIALIZATION(__vectorcall)
#endif // defined(_M_AMD64)

#undef MEMBER_THUNK_GENERATE_CONST_SPECIALIZATION
#undef MEMBER_THUNK_GENERATE_NOEXCEPT_SPECIALIZATION
#undef MEMBER_THUNK_GENERATE_THIS_SPECIALIZATION
#undef MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION
}
