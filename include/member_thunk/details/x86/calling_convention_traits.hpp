#pragma once

namespace member_thunk::details
{
	template<typename Func>
	inline constexpr bool is_this_on_stack_v = false;

	template<typename Ret, typename... Args>
	inline constexpr bool is_this_on_stack_v<Ret(__cdecl*)(Args...)> = true;

	template<typename Ret, typename... Args>
	inline constexpr bool is_this_on_stack_v<Ret(__stdcall*)(Args...)> = true;

	template<typename Func>
	inline constexpr bool is_this_on_register_v = false;

	template<typename Ret, typename... Args>
	inline constexpr bool is_this_on_register_v<Ret(__fastcall*)(Args...)> = true;

	template<typename Ret, typename... Args>
	inline constexpr bool is_this_on_register_v<Ret(__vectorcall*)(Args...)> = true;
}