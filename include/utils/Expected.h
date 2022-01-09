//
// Created by lily on 12/31/21.
//

#ifndef ELFLDR_EXPECTED_H
#define ELFLDR_EXPECTED_H

#include <cstdint>

#include "utils.h"

namespace elfldr::util {

	namespace detail {

		/**
		 * A safe deferred container for class types.
		 * Performs no heap allocations.
		 */
		template <class T>
		struct DeferredHolderFor {
			constexpr ~DeferredHolderFor() {
				if(constructed)
					Get()->~T();
			}

			constexpr DeferredHolderFor() = default; // do not initalize anything

			explicit DeferredHolderFor(const T& t) {
				// call the copy constructor
				Construct(t);
			}

			constexpr DeferredHolderFor& operator=(const DeferredHolderFor& df) {
				if(this == &df)
					return *this;

				(*this->Get()) = (*df->Get());
			}

			constexpr DeferredHolderFor& operator=(const T& t) {
				Construct(t);
			}

			template <class... Args>
			constexpr void Construct(Args&&... args) {
				if(constructed)
					Destruct();

				// construct new T
				constructed = true;
				new(Get()) T(util::Forward<Args>(args)...);
			}

			constexpr void Destruct() {
				if(constructed) {
					constructed = false;
					Get()->~T();
				}
			}

			[[nodiscard]] constexpr bool IsConstructed() const {
				return constructed;
			}

			constexpr T& GetConstructed() {
				ELFLDR_ASSERT(constructed);
				return *Get();
			}

			constexpr const T& GetConstructed() const {
				ELFLDR_ASSERT(constructed);
				return *Get();
			}

		   private:
			constexpr T* Get() {
				return util::UBCast<T*>(&storage[0]);
			}

			constexpr const T* Get() const {
				return util::UBCast<const T*>(&storage[0]);
			}

			bool constructed { false };
			alignas(T) std::uint8_t storage[sizeof(T)] {};
		};

	} // namespace detail

	/**
	 * A class template for working with deterministic errors.
	 * \tparam T Retty. Can be void
	 * \tparam E Error type.
	 */
	template <class T, class E>
	struct Expected {
		constexpr Expected() = default;

		constexpr Expected(const T& t)
			: t(t) {
		}

		constexpr Expected(const E& e)
			: e(e) {
		}

		constexpr Expected& operator=(const T& t) {
			if(e.IsConstructed())
				e.Destruct();

			this->t = t;
			return *this;
		}

		constexpr Expected& operator=(const E& e) {
			if(t.IsConstructed())
				t.Destruct();

			this->e = e;
			return *this;
		}

		inline Expected& operator=(Expected& e) {
			if(this == &e)
				return *this;

			if(e.HasValue())
				this->t = e.Value();

			if(e.HasError())
				this->e = e.Error();

			return *this;
		}

		constexpr bool HasError() const {
			return e.IsConstructed();
		}

		constexpr bool HasValue() const {
			return t.IsConstructed();
		}

		constexpr T& Value() {
			ELFLDR_ASSERT(HasValue() && !HasError());
			return t.GetConstructed();
		}

		constexpr E& Error() {
			ELFLDR_ASSERT(!HasValue() && HasError());
			return e.GetConstructed();
		}

		constexpr const T& Value() const {
			ELFLDR_ASSERT(HasValue() && !HasError());
			return t.GetConstructed();
		}

		constexpr const E& Error() const {
			ELFLDR_ASSERT(!HasValue() && HasError());
			return e.GetConstructed();
		}

	   private:
		detail::DeferredHolderFor<T> t;
		detail::DeferredHolderFor<E> e;
	};

	template <class E>
	struct Expected<void, E> {
		constexpr Expected() = default;

		constexpr Expected(const E& e)
			: e(e) {
		}

		constexpr Expected& operator=(const E& e) {
			this->e = e;
			return *this;
		}

		inline Expected& operator=(Expected& e) {
			if(this == &e)
				return *this;

			if(e.HasError())
				this->e = e.Error();

			return *this;
		}

		constexpr bool HasError() const {
			return e.IsConstructed();
		}

		constexpr E& Error() {
			ELFLDR_ASSERT(HasError());
			return e.GetConstructed();
		}

		constexpr const E& Error() const {
			ELFLDR_ASSERT(HasError());
			return e.GetConstructed();
		}

	   private:
		detail::DeferredHolderFor<E> e;
	};

	template <class E>
	using VoidExpected = Expected<void, E>;

	// Nicer syntax for returning no error from
	// VoidExpected<E> retty-functions
	template <class E>
	constexpr static VoidExpected<E> NO_ERROR {};

} // namespace elfldr::util

#endif // ELFLDR_EXPECTED_H
