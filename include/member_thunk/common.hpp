#pragma once

namespace member_thunk
{
#ifdef __cpp_concepts // MIGRATION: IDE concept support
	template<typename Func>
#else
	template<typename Func, class = void>
#endif
	class thunk;
}