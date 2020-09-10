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

	page<> allocate_page()
	{
		return details::get_default_heap().allocate_page();
	}

	void compact()
	{
		details::get_default_heap().compact();
	}
}

#include "./member_thunk.impl.hpp"
#endif // !defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP)
