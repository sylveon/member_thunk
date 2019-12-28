#pragma once
#include <memory>

#include "common.hpp"
#include "function_traits.hpp"

namespace member_thunk
{
	template<typename Func, typename Class, typename MemberFunc>
#ifdef __cpp_lib_concepts // MIGRATION: IDE concept support
		requires is_compatible_function_types_v<Func, Class, MemberFunc>
#endif
	std::unique_ptr<thunk<Func>> make(Class* pThis, MemberFunc pFunc)
	{
		return std::make_unique<thunk<Func>>(pThis, pFunc);
	}

	template<typename Class, typename MemberFunc>
	struct factory
	{
		Class* pThis;
		MemberFunc pFunc;

		template<typename Func>
#ifdef __cpp_lib_concepts // MIGRATION: IDE concept support
			requires is_compatible_function_types_v<Func, Class, MemberFunc>
#endif
		operator std::unique_ptr<thunk<Func>>()
		{
			return make<Func>(pThis, pFunc);
		}
	};

	template<typename Class, typename MemberFunc>
	factory<Class, MemberFunc> make(Class* pThis, MemberFunc pFunc)
	{
		return { pThis, pFunc };
	}
}

#if defined(_M_IX86)
#include "x86/x86_stack_thunk.hpp"
#include "x86/x86_register_thunk.hpp"
#elif defined(_M_AMD64)
#include "x64_thunk.hpp"
#elif defined(_M_ARM)
#error "Target architecture not supported"
#elif defined(_M_ARM64)
#error "Target architecture not supported"
#endif