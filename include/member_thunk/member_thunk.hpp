#pragma once

#if !defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP)
#	include "./default_heap.impl.hpp"
#endif // !defined(MEMBER_THUNK_DISABLE_DEFAULT_HEAP)

#include "./details/heap/memory.impl.hpp"
#include "./details/heap/region.impl.hpp"
#include "./details/list.impl.hpp"
#include "./heap.impl.hpp"
#include "./page.impl.hpp"

#if defined(_M_AMD64)
#	include "./details/thunk/thunk.impl.x64.hpp"
#elif defined(_M_ARM64)
#	include "./details/thunk/thunk.impl.arm64.hpp"
#else
#	error "Target architecture not supported"
#endif
