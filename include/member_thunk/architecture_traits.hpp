#pragma once

namespace member_thunk
{
	enum class architecture
	{
		x86,
		x64,
		arm64,
#if defined(_M_IX86)
		native = x86
#elif defined(_M_AMD64)
		native = x64
#elif defined(_M_ARM64)
		native = arm64
#endif
	};

	template<architecture arch>
	inline constexpr bool is_architecture_v = architecture::native == arch;
}