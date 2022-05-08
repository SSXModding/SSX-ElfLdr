/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef MLSTD_OPTIONAL_H
#define MLSTD_OPTIONAL_H

#include <mlstd/detail/DeferredHolder.h>

namespace mlstd {

	/**
	 * Safe optional object.
	 */
	template <class T>
	struct Optional {
		using ValueType = T;
		using Reference = T&;
		using ConstReference = const T&;

		constexpr Optional() = default;

		constexpr Optional(const T& t) // NOLINT
			: value(t) {
		}

		constexpr Optional& operator=(const T& t) {
			value = t;
			return *this;
		}

		inline Optional& operator=(const Optional& optional) {
			if(this == &optional)
				return *this;

			value = optional.value;
			return *this;
		}

		constexpr bool HasValue() const {
			return value.IsConstructed();
		}

		constexpr T& Value() {
			MLSTD_VERIFY(HasValue());
			return value.GetConstructed();
		}

		constexpr const T& Value() const {
			MLSTD_VERIFY(HasValue());
			return value.GetConstructed();
		}

		T& operator*() {
			return Value();
		}

		const T& operator*() const {
			return Value();
		}

		T* operator->() {
			return &Value();
		}

		const T* operator->() const {
			return &Value();
		}

	   private:
		detail::DeferredHolder<T> value;
	};

} // namespace mlstd

#endif // ELFLDR_OPTIONAL_H
