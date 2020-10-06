#pragma once
#if !defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP)
#	include "./default_heap.hpp"

#	include "./heap.hpp"
#	include "./page.hpp"

#	if MEMBER_THUNK_DEFAULT_HEAP_SAFETY == 2
#		define MEMBER_THUNK_DEFAULT_HEAP_LIFETIME thread_local
#	else
#		define MEMBER_THUNK_DEFAULT_HEAP_LIFETIME
#	endif // MEMBER_THUNK_DEFAULT_HEAP_SAFETY == 2

namespace member_thunk
{
	inline heap<details::default_lock_t>& details::get_default_heap()
	{
		static MEMBER_THUNK_DEFAULT_HEAP_LIFETIME heap<default_lock_t> default_heap;
		return default_heap;
	}

	inline page<details::default_lock_t> allocate_page() { return details::get_default_heap().allocate_page(); }
	inline void compact() { details::get_default_heap().compact(); }
}
#endif // !defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP)
