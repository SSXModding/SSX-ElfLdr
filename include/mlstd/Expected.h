/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef MLSTD_EXPECTED_H
#define MLSTD_EXPECTED_H

#include <mlstd/Assert.h>
#include <mlstd/detail/DeferredHolder.h>
#include <mlstd/Utility.h>
#include <stdint.h>

namespace mlstd {

	/**
	 * A class template for working with deterministic errors.
	 * \tparam T Return type. Can be void
	 * \tparam E Error type.
	 */
	template <class T, class E>
	struct Expected {
		constexpr Expected() = default;

		constexpr Expected(const T& t) // NOLINT
			: value(t) {
		}

		constexpr Expected(const E& e) // NOLINT
			: error(e) {
		}

		constexpr Expected& operator=(const T& t) {
			if(error.IsConstructed())
				error.Destruct();

			this->value = t;
			return *this;
		}

		constexpr Expected& operator=(const E& e) {
			if(value.IsConstructed())
				value.Destruct();

			this->error = e;
			return *this;
		}

		inline Expected& operator=(const Expected& e) {
			if(this == &e)
				return *this;

			if(e.HasValue())
				this->t = e.Value();

			if(e.HasError())
				this->error = e.Error();

			return *this;
		}

		[[nodiscard]] constexpr bool HasError() const {
			return error.IsConstructed();
		}

		[[nodiscard]] constexpr bool HasValue() const {
			return value.IsConstructed();
		}

		constexpr T& Value() {
			MLSTD_VERIFY(HasValue() && !HasError());
			return value.GetConstructed();
		}

		constexpr E& Error() {
			MLSTD_VERIFY(!HasValue() && HasError());
			return error.GetConstructed();
		}

		constexpr const T& Value() const {
			MLSTD_VERIFY(HasValue() && !HasError());
			return value.GetConstructed();
		}

		constexpr const E& Error() const {
			MLSTD_VERIFY(!HasValue() && HasError());
			return error.GetConstructed();
		}

		// Dereference operators.
		// Make sure to ensure that the Expected has a value when using these functions

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
		detail::DeferredHolder<E> error;
	};

	template <class E>
	struct Expected<void, E> {
		constexpr Expected() = default;

		constexpr Expected(const E& e) // NOLINT
			: error(e) {
		}

		constexpr Expected& operator=(const E& e) {
			this->error = e;
			return *this;
		}

		inline Expected& operator=(const Expected& e) {
			if(this == &e)
				return *this;

			if(e.HasError())
				this->error = e.Error();

			return *this;
		}

		[[nodiscard]] constexpr bool HasError() const {
			return error.IsConstructed();
		}

		constexpr E& Error() {
			MLSTD_ASSERT(HasError());
			return error.GetConstructed();
		}

		constexpr const E& Error() const {
			MLSTD_ASSERT(HasError());
			return error.GetConstructed();
		}

	   private:
		detail::DeferredHolder<E> error;
	};

	template <class E>
	using VoidExpected = Expected<void, E>;

	// Allows for nicer syntax for returning no error from
	// VoidExpected<E> functions.

	template <class E>
	constexpr static VoidExpected<E> NO_ERROR {};

} // namespace mlstd

#endif // MLSTD_EXPECTED_H
