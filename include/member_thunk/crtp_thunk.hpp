#pragma once
#include "base_thunk.hpp"

namespace member_thunk
{
	template<typename Derived, typename Func>
	class crtp_thunk : public base_thunk
	{
	private:
		void flush()
		{
			base_thunk::flush(static_cast<Derived*>(this), sizeof(Derived));
		}

		void set_call_target(bool valid)
		{
			base_thunk::set_call_target(static_cast<Derived*>(this), sizeof(Derived), valid);
		}

	protected:
		void init_thunk()
		{
			flush();
			set_call_target(true);
		}

		~crtp_thunk() noexcept(false)
		{
			set_call_target(false);
		}

	public:
		Func get_thunked_function() const
		{
			return reinterpret_cast<Func>(static_cast<const Derived*>(this));
		}
	};
}