#pragma once
#include <tuple>
#include <type_traits>

namespace member_thunk
{
	template<typename T>
	inline constexpr bool is_function_pointer_with_pointer_sized_first_arg_v = false;

	template<typename Ret, typename T, typename... Args>
	inline constexpr bool is_function_pointer_with_pointer_sized_first_arg_v<Ret(*)(T, Args...)> = sizeof(T) == sizeof(void*);

	template<typename T, typename Class, typename U>
	inline constexpr bool is_class_member_function_pointer_with_same_args_except_first_v = false;

#define MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(...) \
	template<typename Ret, typename Class, typename T, typename... Args> \
	inline constexpr bool is_class_member_function_pointer_with_same_args_except_first_v<Ret(*)(T, Args...), Class, Ret(Class::*)(Args...) __VA_ARGS__> = true

	MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION();
	MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION(const);
#undef MEMBER_THUNK_GENERATE_MEMBER_SPECIALIZATION
}