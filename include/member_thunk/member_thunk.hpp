#pragma once
#include <memory>

#include "common.hpp"
#include "details/function_traits.hpp"

namespace member_thunk
{
	template<typename Func, typename MemberFunc>
	requires details::is_compatible_function_types_v<Func, MemberFunc>
	std::unique_ptr<thunk<Func>> make(details::get_this_ptr_t<MemberFunc> pThis, MemberFunc pFunc)
	{
		return std::make_unique<thunk<Func>>(const_cast<void*>(static_cast<const void*>(pThis)), reinterpret_cast<void*&>(pFunc));
	}

	namespace details
	{
		template<typename MemberFunc>
		struct factory
		{
			get_this_ptr_t<MemberFunc> that;
			MemberFunc function;

			template<typename Func>
			requires is_compatible_function_types_v<Func, MemberFunc>
			operator std::unique_ptr<thunk<Func>>()
			{
				return make<Func>(that, function);
			}
		};
	}

	template<typename MemberFunc>
	details::factory<MemberFunc> make(details::get_this_ptr_t<MemberFunc> pThis, MemberFunc pFunc)
	{
		return { pThis, pFunc };
	}
}

#if defined(_M_IX86)
# include "details/x86/x86_stack_thunk.hpp"
# include "details/x86/x86_register_thunk.hpp"
#elif defined(_M_AMD64)
# include "details/x64_thunk.hpp"
#elif defined(_M_ARM64)
# include "details/arm64_thunk.hpp"
#else
# error "Target architecture not supported"
#endif