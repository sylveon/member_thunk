#pragma once
#include "./member_thunk.hpp"

#if MEMBER_THUNK_DEFAULT_HEAP_SAFETY == 2
#define MEMBER_THUNK_DEFAULT_HEAP_LIFETIME thread_local
#else
#define MEMBER_THUNK_DEFAULT_HEAP_LIFETIME
#endif // MEMBER_THUNK_DEFAULT_HEAP_SAFETY == 2

namespace member_thunk::details
{
	heap<>& get_default_heap()
	{
		static MEMBER_THUNK_DEFAULT_HEAP_LIFETIME heap default_heap;
		return default_heap;
	}
}