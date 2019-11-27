#pragma once
#include <memory>

#include "common.hpp"
#include "function_traits.hpp"

namespace member_thunk
{
	template<typename Func, typename Class, typename MemberFunc>
		requires is_compatible_function_types_v<Func, Class, MemberFunc>
	std::unique_ptr<thunk<Func>> make(Class* pThis, MemberFunc pFunc)
	{
		return std::make_unique<thunk<Func>>(pThis, pFunc);
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