#pragma once

#if !defined(MEMBER_THUNK_DEFAULT_HEAP_SAFETY)
#	define MEMBER_THUNK_DEFAULT_HEAP_SAFETY 3
#endif // !defined(MEMBER_THUNK_DEFAULT_HEAP_SAFETY)

#if defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP) || MEMBER_THUNK_DEFAULT_HEAP_SAFETY != 3
#	include "./lock/null_lock.hpp"

namespace member_thunk::details
{
	using default_lock_t = null_lock;
}
#else
#	include "./lock/slim_lock.hpp"

namespace member_thunk::details
{
	using default_lock_t = slim_lock;
}
#endif // defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP) || MEMBER_THUNK_DEFAULT_HEAP_SAFETY != 3

#if !defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP)
namespace member_thunk
{
	template<typename T>
	class heap;

	class page;

	namespace details
	{
		heap<default_lock_t>& get_default_heap();
	}

	page allocate_page();
	void compact();
}
#endif // !defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP)
