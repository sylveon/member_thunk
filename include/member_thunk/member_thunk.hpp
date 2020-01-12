#pragma once
#include <memory>

#include "common.hpp"
#include "function_traits.hpp"

namespace member_thunk
{
	template<typename Func, typename MemberFunc>
#ifdef __cpp_concepts // MIGRATION: IDE concept support
		requires is_compatible_function_types_v<Func, MemberFunc>
#endif
	std::unique_ptr<thunk<Func>> make(get_this_ptr_t<MemberFunc> pThis, MemberFunc pFunc)
	{
		return std::make_unique<thunk<Func>>(const_cast<void*>(static_cast<const void*>(pThis)), reinterpret_cast<void*&>(pFunc));
	}

	template<typename MemberFunc>
	struct factory
	{
		get_this_ptr_t<MemberFunc> that;
		MemberFunc function;

		template<typename Func>
#ifdef __cpp_concepts // MIGRATION: IDE concept support
			requires is_compatible_function_types_v<Func, MemberFunc>
#endif
		operator std::unique_ptr<thunk<Func>>()
		{
			return make<Func>(that, function);
		}
	};

	template<typename MemberFunc>
	factory<MemberFunc> make(get_this_ptr_t<MemberFunc> pThis, MemberFunc pFunc)
	{
		return { pThis, pFunc };
	}
}

#if defined(_M_IX86)
# include "x86/x86_stack_thunk.hpp"
# include "x86/x86_register_thunk.hpp"
#elif defined(_M_AMD64)
# include "x64_thunk.hpp"
#elif defined(_M_ARM64)
# include "arm64_thunk.hpp"
#else
# error "Target architecture not supported"
#endif