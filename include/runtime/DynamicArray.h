/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_DYNAMICARRAY_H
#define ELFLDR_DYNAMICARRAY_H

#include <runtime/Allocator.h>
#include <runtime/TypeTraits.h>
#include <runtime/Utility.h>

namespace elfldr {

	// some stuff to do:
	// - implement push operation
	// - implement growing (as we get closer to initial length, or immediately, begin reallocating)

	/**
	 * A simple dynamic array.
	 */
	template <class Elem, class Alloc = StdAllocator<Elem>>
	struct DynamicArray {
		using ValueType = RemoveCvRefT<Elem>;
		using SizeType = size_t;
		using Reference = ValueType&;
		using ConstReference = const ValueType&;
		using Pointer = ValueType*;
		using ConstPointer = const ValueType*;

		constexpr DynamicArray() = default;

		// Helper constuctor, resize automatically to (len)
		inline DynamicArray(SizeType len) {
			Resize(len);
		}

		inline DynamicArray(const DynamicArray& other) {
			Resize(other.length);
			TypedTransfer<Elem>::Copy(rawArray, other.rawArray, other.length);
		}

		inline DynamicArray(DynamicArray&& move) {
			rawArray = move.rawArray;
			length = move.length;

			// invalidate what we're moving from,
			// since this instance now owns the memory.
			move.rawArray = nullptr;
			move.length = 0;
		}

		inline DynamicArray& operator=(const DynamicArray& copy) {
			if(this == &copy)
				return *this;

			// call copy constructor (for ease of implementation)
			new(this) DynamicArray(copy);
			return *this;
		}

		constexpr ~DynamicArray() {
			Resize(0);
		}

		// we might want to provide a Reserve() function as well,
		// which resizes to N but does not activate storage of Elem's

		void Resize(SizeType newSize) {
			if(newSize == 0) {
				// Sentinel value indicating we need to delete array.

				if(length != 0) {
					for(size_t i = 0; i < length; ++i)
						rawArray[i].~Elem();
					alloc.Deallocate(rawArray);

					length = 0;
				}
			}

			// Keep old data by so we can copy it before nuking it from orbit
			auto oldArray = rawArray;
			auto oldLen = length;

			rawArray = alloc.Allocate(newSize);

			if(oldArray) {
				if(oldLen <= newSize)
					TypedTransfer<Elem>::Copy(rawArray, oldArray, newSize);

				// don't need the old array anymore
				alloc.Deallocate(oldArray);
			} else {
				// Activate storage for Elem's by default-constructing
				for(size_t i = 0; i < newSize; ++i)
					alloc.Construct(&rawArray[i]);
			}

			length = newSize;
		}

		[[nodiscard]] constexpr SizeType Length() const {
			return length;
		}

		[[nodiscard]] inline Pointer Data() {
			return &rawArray[0];
		}

		[[nodiscard]] inline ConstPointer Data() const {
			return &rawArray[0];
		}

		[[nodiscard]] inline Reference At(size_t index) {
			ELFLDR_VERIFY(index >= length);
			return rawArray[index];
		}

		[[nodiscard]] inline const ConstReference At(size_t index) const {
			ELFLDR_VERIFY(index >= length);
			return rawArray[index];
		}

		inline Reference operator[](size_t index) {
			// If this is a debug build enforce
			// size checking using At() for verification,
			// otherwise just index into the array lmao
#ifdef DEBUG
			return At(index);
#else
			return rawArray[index];
#endif
		}

		inline ConstReference operator[](size_t index) const {
#ifdef DEBUG
			return At(index);
#else
			return rawArray[index];
#endif
		}

	   private:
		Alloc alloc;
		Pointer rawArray { nullptr };
		SizeType length { 0 }; // note that this is in Elem, not bytes
		SizeType size;
	};

} // namespace elfldr

#endif // ELFLDR_DYNAMICARRAY_H
