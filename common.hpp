#pragma once

#define MEMBER_THUNK_PACK __pragma(pack(push, 1))
#define MEMBER_THUNK_UNPACK __pragma(pack(pop))

#define MEMBER_THUNK_ASSERT_SIZE(T, s) static_assert(sizeof(T) == s, "Thunk class is bigger than assembly")
#define MEMBER_THUNK_ASSERT_ARCHITECTURE(expected) static_assert(member_thunk::architecture::expected == member_thunk::architecture::native, "Tried to create a thunk for a mismatching architecture")

#ifdef __cpp_lib_concepts
#define MEMBER_THUNK_REQUIRES(requirement) requires requirement
#else
#define MEMBER_THUNK_REQUIRES(requirement)
#endif

namespace member_thunk
{
	enum class architecture
	{
		x86,
		x64,
		arm,
		arm64,
#if defined(_M_IX86)
		native = x86
#elif defined(_M_AMD64)
		native = x64
#elif defined(_M_ARM)
		native = arm
#elif defined(_M_ARM64)
		native = arm64
#endif
	};
}