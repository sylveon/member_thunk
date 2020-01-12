#include "base_thunk.hpp"

namespace member_thunk
{
	template<typename Derived, typename Func>
	class crtp_thunk : public base_thunk
	{
	protected:
		void flush()
		{
			base_thunk::flush(static_cast<Derived*>(this), sizeof(Derived));
		}

	public:
		Func get_thunked_function() const
		{
			return reinterpret_cast<Func>(static_cast<const Derived*>(this));
		}
	};
}