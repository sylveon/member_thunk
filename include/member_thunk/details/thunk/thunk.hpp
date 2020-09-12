#pragma once
#include <concepts>
#include <cstddef>
#include <cstdint>

namespace member_thunk
{
	template<typename T>
	class page;
}

#pragma pack(push, 1)
namespace member_thunk::details
{
	class alignas(16) thunk final
	{
		template<typename T>
		friend class page;

#if defined(_M_AMD64)
		// mov rax, {function}
		// mov rcx, {this}
		// rex_jmp rax
		// int 3
		std::uint8_t mov_rax[2];
		void* function;
		std::uint8_t mov_rcx[2];
		void* that;
		std::uint8_t rex_jmp_rax[3];
		std::uint8_t int_3[9];
#elif defined(_M_ARM64)
		static constexpr std::uint8_t BREAK[] = { 0x00, 0x00, 0x3E, 0xD4 };

		// ldr x8, #16
		// ldr x0, #20
		// br x8
		// brk #0xF000
		// {function}
		// {this}
		std::uint8_t ldr_x8_16[4];
		std::uint8_t ldr_x0_20[4];
		std::uint8_t br_x8[4];
		std::uint8_t brk_F000[4];
		void* function;
		void* that;
#endif

		thunk() noexcept = default;
		thunk(void* that, void* func) noexcept;

		thunk(const thunk&) = delete;
		thunk& operator=(const thunk&) = delete;

		// private placement new
		constexpr void* operator new(std::size_t, void* ptr) noexcept
		{
			return ptr;
		}

		~thunk() noexcept;
	};
}
#pragma pack(pop)
