#pragma once

namespace member_thunk
{
#ifdef __cpp_lib_concepts // MIGRATION: IDE concept support
	template<typename Func>
#else
	template<typename Func, class = void>
#endif
	class thunk;
}