#pragma once

#define MEMBER_THUNK_PACK __pragma(pack(push, 1))
#define MEMBER_THUNK_UNPACK __pragma(pack(pop))

#define MEMBER_THUNK_STATIC_ASSERT_SIZEOF_THIS(s) static_assert(sizeof(*this) == s, "Thunk class is bigger than assembly")

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

	template<typename Func, architecture Arch = architecture::native>
	class thunk;
}