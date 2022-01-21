/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_STRING_H
#define ELFLDR_STRING_H

#include <utils/Allocator.h>

#include <cstdint>
#include <cstring>

// :(
#include <utils/Hash.h>

namespace elfldr::util {

	/**
	 * A "view" of a string. Does not own the memory,
	 * and when copied, simply copies its pointer/length.
	 * Essentially a special-cased span for strings,
	 * to avoid creating tons of String objects which manage their memory on the heap.
	 */
	template <class T>
	struct BasicStringView {
		constexpr BasicStringView()
			: data_ptr(nullptr),
			  len(0) {
		}

		// Permit the compiler to force generate a trivial
		// copy constructor.
		constexpr BasicStringView(const BasicStringView&) = default;

		constexpr BasicStringView(const T* ptr)
			: data_ptr(ptr),
			  len(strlen(ptr)) {
		}

		constexpr BasicStringView(T* ptr, size_t len)
			: data_ptr(ptr),
			  len(len) {
		}

		constexpr size_t Length() const {
			return len;
		}

		constexpr const T* Data() const {
			return data_ptr;
		}

		constexpr const T* CStr() const {
			return data_ptr;
		}

		constexpr const T& operator[](std::size_t index) const {
			return &data_ptr[index];
		}

		friend constexpr bool operator==(const BasicStringView& lhs, const BasicStringView& rhs) {
			// would probably need some work for introducing U8StringView
			return !strcmp(lhs.data_ptr, rhs.data_ptr);
		}

		friend constexpr bool operator!=(const BasicStringView& lhs, const BasicStringView& rhs) {
			return !(lhs == rhs);
		}

	   private:
		const T* data_ptr;
		size_t len;
	};

	// maybe later: CharTraits<T>?

	template <class T, class Alloc = util::StdAllocator<T>>
	struct BasicString {
		using CharType = T;
		using SizeType = std::size_t;

		inline BasicString() = default;

		inline BasicString(const T* cstr) {
			CopyFromCString(cstr);
		}

		inline BasicString(const T* mem, int length) {
			if(!mem)
				return;

			// TODO: maybe some interning.
			// 32 chars max intern, before it becomes an allocation.
			Resize(length);
			memcpy(&memory[0], &mem[0], length * sizeof(T));
		}

		inline BasicString(BasicString&& move) {
			memory = move.memory;
			len = move.len;

			// invalidate the string we're moving from,
			// since this instance now owns the memory.

			move.memory = nullptr;
			move.len = 0;
		}

		inline BasicString(const BasicString& source) {
			// new buffer.
			Resize(source.len);
			memcpy(&memory[0], &source.memory[0], source.len * sizeof(T));
		}

		inline BasicString& operator=(const BasicString& copy) {
			if(this == &copy)
				return *this;

			// call Zecopyctor
			new(this) BasicString(copy);
			return *this;
		}

		inline ~BasicString() {
			Resize(0);
		}

		inline const T* c_str() const {
			return memory;
		}

		[[nodiscard]] inline SizeType length() const {
			return len;
		}

		inline T& operator[](std::size_t index) {
			return memory[index];
		}

		inline const T& operator[](std::size_t index) const {
			return memory[index];
		}

		inline T* data() {
			return memory;
		}

		inline const T* data() const {
			return memory;
		}

		inline void Resize(SizeType newLength) {
			if(newLength == 0) {
				// Destroy the buffer, if we have one to destroy
				if(memory) {
					len = 0;
					alloc.Deallocate(memory);
				}
				return;
			}

			// save old buffer
			auto* old = memory;

			// allocate new buffer
			memory = alloc.Allocate(newLength + 1);
			memory[newLength] = '\0';

			if(old) {
				// copy the old buffer in
				// TODO: could probably maybe truncate for any length?
				if(len <= newLength)
					memcpy(&memory[0], &old[0], len * sizeof(T));

				// don't need it
				alloc.Deallocate(old);
			}
			len = newLength;
		}

		// TODO (maybe?):
		//
		// BasicString substr(SizeType pos, SizeType len) - Returns a new allocated substring of this source string

		inline BasicString substr(SizeType pos, SizeType len = -1) {
			if(pos > this->len)
				return "";

			if(len != -1) {
				if((pos + len) > this->len)
					return "";
			} else {
				len = 0;

				auto d = &data()[pos];

				// let's try to find a null terminator
				while(*d != '\0') {
					// If we weren't able to find the terminator, then give up,
					// and just say it's the end of the string.
					if(d == &data()[this->len]) {
						len = pos - this->len;
						break;
					}

					++len;
					++d;
				}
			}

			return BasicString(&data()[pos], len);
		}

		// equality operators
		// operator== might need some work done to it.

		friend inline bool operator==(const BasicString& lhs, const BasicString& rhs) {
			// would probably need some work for introducing U8String
			return !strcmp(lhs.data(), rhs.data());
		}

		friend inline bool operator!=(const BasicString& lhs, const BasicString& rhs) {
			return !(lhs == rhs);
		}

		explicit operator BasicStringView<T>() {
			return BasicStringView<T>(data(), length());
		}

	   private:
		void CopyFromCString(const T* cstr) {
			if(!cstr)
				return;

			const auto clen = strlen(cstr);

			Resize(clen);
			memcpy(&memory[0], &cstr[0], clen * sizeof(T));
		}

		T* memory { nullptr };
		SizeType len {};
		Alloc alloc;
	};

	// Hash<T> specializations for string stuff,
	// this'll automatically work with U8String and co

	template <class CharT>
	struct Hash<BasicString<CharT>> {
		inline static std::uint32_t hash(const BasicString<CharT>& str) {
			return util::fnv1a_hash(UBCast<void*>(str.data()), str.length() * sizeof(CharT), 0);
		}
	};

	template <class CharT>
	struct Hash<BasicStringView<CharT>> {
		inline static std::uint32_t hash(const BasicStringView<CharT>& str) {
			return util::fnv1a_hash(UBCast<void*>(str.Data()), str.Length() * sizeof(CharT), 0);
		}
	};

	using String = BasicString<char>;
	using StringView = BasicStringView<char>;

	// safe for UTF-8 (?)
	// using U8String = BasicString<char8_t>;
	// using U8StringView = BasicStringView<char8_t>;

} // namespace elfldr

#endif