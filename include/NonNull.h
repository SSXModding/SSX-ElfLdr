

namespace elfldr::util {

	namespace detail {

		/**
		 * C++11 onwards declares that
		 * decltype(nullptr) will result in a unique type, which only nullptr will resolve to.
		 *
		 * To avoid annoyance (just like the real standard library),
		 * we create a using, to avoid putting the same decltype expression
		 * everywhere.
		 */
		using nullptr_t = decltype(nullptr);

	} // namespace detail

	template <class T>
	struct NonNull {
		static_assert(!IsSameV<T, detail::nullptr_t>, "no");

		// destroy construction/assignment for nullptr_t
		// so that `auto n = NonNull<int*>(nullptr)` fails compilation
		//
		NonNull(detail::nullptr_t) = delete;
		NonNull& operator=(detail::nullptr_t) = delete;
	}
} // namespace elfldr::util