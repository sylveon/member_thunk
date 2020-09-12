#pragma once
#include "./details/common.hpp"
#include "./heap.hpp"
#include "./page.hpp"

#if !defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP)
namespace member_thunk
{
	namespace details
	{
		heap<>& get_default_heap();
	}

	inline page<> allocate_page()
	{
		return details::get_default_heap().allocate_page();
	}

	inline void compact()
	{
		details::get_default_heap().compact();
	}
}

#include "./member_thunk.impl.hpp"
#endif // !defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP)

#include "./heap.impl.hpp"
#include "./page.impl.hpp"
#include "./details/heap/region.impl.hpp"

#if defined(_M_AMD64)
# include "./details/thunk/thunk.impl.x64.hpp"
#elif defined(_M_ARM64)
# include "./details/thunk/thunk.impl.arm64.hpp"
#else
# error "Target architecture not supported"
#endif
