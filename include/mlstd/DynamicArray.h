/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef MLSTD_DYNAMICARRAY_H
#define MLSTD_DYNAMICARRAY_H

#include <mlstd/Allocator.h>
#include <mlstd/TypeTraits.h>
#include <mlstd/Utility.h>

namespace mlstd {

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
		explicit inline DynamicArray(SizeType len) {
			Resize(len);
		}

		inline DynamicArray(const DynamicArray& other) {
			Resize(other.length);
			TypedTransfer<Elem>::Copy(rawArray, other.rawArray, other.length);
		}

		inline DynamicArray(DynamicArray&& move) noexcept {
			rawArray = move.rawArray;
			capacity = move.length;
			size = move.size;

			// invalidate what we're moving from,
			// since this instance now owns the memory.
			move.rawArray = nullptr;
			move.length = 0;
			move.size = 0;
		}

		constexpr ~DynamicArray() {
			Resize(0);
		}

		inline DynamicArray& operator=(const DynamicArray& copy) {
			if(this == &copy)
				return *this;

			// call copy constructor (for ease of implementation)
			new(this) DynamicArray(copy);
			return *this;
		}

		// move assignment (just move the buffer to us)
		// then rule of 5 should be relatively there

		// we might want to provide a Reserve() function as well,
		// which resizes to N but does not activate storage of Elem's

		void Reserve(SizeType newCapacity) {
			auto destroy = [&](Elem* ptr, SizeType size) {
				for(SizeType i = 0; i < size; ++i)
					ptr[i].~Elem();
				alloc.Deallocate(ptr);
			};

			if(newCapacity == 0) {
				// Sentinel value indicating we need to delete array.
				if(capacity != 0) {
					destroy(rawArray, capacity);
					capacity = 0;
					size = 0;
					return;
				}
			}

			if(newCapacity < capacity) {
				return;
			}

			// Keep old data so we can copy it before nuking it from orbit
			auto oldArray = rawArray;
			auto oldCapacity = capacity;

			rawArray = alloc.Allocate(newCapacity);

			if(oldArray) {
				// Copy the old data to the new buffer
				TypedTransfer<Elem>::Copy(rawArray, oldArray, oldCapacity);

				// Initalize newly reserved elements by default construction
				for(SizeType i = oldCapacity; i < newCapacity; ++i)
					alloc.Construct(&rawArray[i]);

				// don't need the old array anymore
				destroy(oldArray, oldCapacity);
			} else {
				// Activate storage lifetime by default-construction
				for(SizeType i = 0; i < newCapacity; ++i)
					alloc.Construct(&rawArray[i]);
			}

			capacity = newCapacity;
		}

		 void Resize(SizeType newSize) {
            if(newSize >= capacity) {
                Reserve(newSize + 4);
			} else {
				// Destroy
				for(SizeType i = newSize; i < size; ++i)
                 rawArray[i].~Elem();
			}

            size = newSize;
        }

		void PushBack(const Elem& elem) {
            Resize(size + 1);
            rawArray[size-1] = elem;
        }

		[[nodiscard]] constexpr SizeType Capacity() const {
			return capacity;
		}

		[[nodiscard]] constexpr SizeType Size() const {
			return size;
		}

		[[nodiscard]] inline Pointer Data() {
			return &rawArray[0];
		}

		[[nodiscard]] inline ConstPointer Data() const {
			return &rawArray[0];
		}

		[[nodiscard]] inline Reference At(size_t index) {
			MLSTD_VERIFY(index >= size);
			return rawArray[index];
		}

		[[nodiscard]] inline ConstReference At(size_t index) const {
			MLSTD_VERIFY(index >= size);
			return rawArray[index];
		}

		inline Reference operator[](size_t index) {
			// If this is a debug build enforce
			// size checking using At() for verification,
			// otherwise just index into the array
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
		[[no_unique_address]] Alloc alloc;
		Pointer rawArray { nullptr };
		SizeType capacity { 0 }; // note that this is in Elem, not bytes
		SizeType size { 0 };
	};

} // namespace mlstd

#endif // MLSTD_DYNAMICARRAY_H
