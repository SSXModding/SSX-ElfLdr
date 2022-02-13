/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_DYNAMICARRAY_H
#define ELFLDR_DYNAMICARRAY_H

#include <runtime/Allocator.h>
#include <runtime/Utility.h>

namespace elfldr {

	/**
	 * A simple dynamic array.
	 */
	template <class Elem, class Alloc = StdAllocator<Elem>>
	struct DynamicArray {
		using SizeType = size_t;
		using Reference = Elem&;
		using ConstReference = const Elem&;
		using Pointer = Elem*;
		using ConstPointer = const Elem*;

		constexpr DynamicArray() = default;

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

		~DynamicArray() {
			Resize(0);
		}

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
			}

			length = newSize;
		}

		SizeType Length() const {
			return length;
		}

		Pointer Data() {
			return &rawArray[0];
		}

		ConstPointer Data() const {
			return &rawArray[0];
		}

		Reference At(size_t index) {
			ELFLDR_VERIFY(index >= length);
			return rawArray[index];
		}

		ConstReference At(size_t index) const {
			ELFLDR_VERIFY(index >= length);
			return rawArray[index];
		}

		Reference operator[](size_t index) {
			// If this is a debug build enforce
			// size checking using At() for verification,
			// otherwise just index into the array lmao
#ifdef DEBUG
			return At(index);
#else
			return rawArray[index];
#endif
		}

		ConstReference operator[](size_t index) const {
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
	};

} // namespace elfldr

#endif // ELFLDR_DYNAMICARRAY_H
