#pragma once

#define MEMBER_THUNK_PACK __pragma(pack(push, 1))
#define MEMBER_THUNK_UNPACK __pragma(pack(pop))

#define MEMBER_THUNK_ASSERT_SIZE(T, s) static_assert(sizeof(T) == s, "Thunk class is bigger than assembly.")

#ifdef __cpp_lib_concepts
#define MEMBER_THUNK_REQUIRES(x) requires x
#else
#define MEMBER_THUNK_REQUIRES(x)
#endif