#pragma once
#include "base_thunk.hpp"

namespace member_thunk::details
{
	template<typename Derived, typename Func>
	class crtp_thunk : public base_thunk
	{
		void flush()
		{
			base_thunk::flush(static_cast<Derived*>(this), sizeof(Derived));
		}

		void set_call_target(bool valid)
		{
			base_thunk::set_call_target(static_cast<Derived*>(this), sizeof(Derived), valid);
		}

	protected:
		template<std::size_t size>
		void init_thunk()
		{
			static_assert(sizeof(Derived) == size, "Thunk class does not have expected size");
			static_assert(alignof(Derived) == thunk_alignment, "Thunk class does not have expected alignment");

			flush();
			set_call_target(true);
		}

		~crtp_thunk() noexcept(false)
		{
			set_call_target(false);
		}

	public:
		Func get_thunked_function() const noexcept
		{
			return reinterpret_cast<Func>(static_cast<const Derived*>(this));
		}
	};
}