/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_DEFERREDHOLDER_H
#define ELFLDR_DEFERREDHOLDER_H

#include <stdint.h>
#include <mlstd/Assert.h>

namespace mlstd::detail {

	/**
	 * A safe deferred container for class types.
	 * Performs no heap allocation - everything is stored in this object,
	 * no buts.
	 */
	template <class T>
	struct DeferredHolder {
		constexpr DeferredHolder() = default; // do not initialize anything

		/**
		 * Constructor for value-initializing.
		 * Note that this calls the copy constructor.
		 */
		explicit DeferredHolder(const T& t) {
			// call the copy constructor
			Construct(t);
		}

		constexpr ~DeferredHolder() {
			if(constructed)
				Get()->~T();
		}

		constexpr DeferredHolder& operator=(const DeferredHolder& df) {
			if(this == &df)
				return *this;

			// If the other object has a value, copy it to us.
			// If not then we do nothing.

			if(df.IsConstructed())
				(*this->Get()) = (*df->Get());

			return *this;
		}

		constexpr DeferredHolder& operator=(const T& t) {
			Construct(t);
		}

		template <class... Args>
		constexpr void Construct(Args&&... args) {
			if(constructed)
				Destruct();

			// construct new T
			constructed = true;
			new(Get()) T(Forward<Args>(args)...);
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
			return reinterpret_cast<T*>(&storage[0]);
		}

		constexpr const T* Get() const {
			return reinterpret_cast<const T*>(&storage[0]);
		}

		bool constructed { false };
		alignas(T) uint8_t storage[sizeof(T)] {};
	};

} // namespace elfldr::detail

#endif // ELFLDR_DEFERREDHOLDER_H
