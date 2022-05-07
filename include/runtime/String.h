/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_STRING_H
#define ELFLDR_STRING_H

#include <runtime/Allocator.h>
#include <runtime/CharTraits.h>
#include <runtime/Hash.h>

namespace elfldr {

	/**
	 * A "view" of a string. Does not own the memory,
	 * and when copied, simply copies its pointer/length.
	 * Essentially a special-cased span for strings,
	 * to avoid creating tons of String objects which manage their memory on the heap.
	 */
	template <class T, class Traits = CharTraits<T>>
	struct BasicStringView {

		using CharType = T;
		using SizeType = size_t;

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

		constexpr BasicStringView(T* ptr, SizeType len)
			: data_ptr(ptr),
			  len(len) {
		}

		[[nodiscard]] constexpr SizeType Length() const {
			return len;
		}

		constexpr const T* Data() const {
			return data_ptr;
		}

		constexpr const T* CStr() const {
			return data_ptr;
		}

		constexpr const T& operator[](SizeType index) const {
			return data_ptr[index];
		}

		friend constexpr bool operator==(const BasicStringView& lhs, const BasicStringView& rhs) {
			return !Traits::Compare(lhs.data_ptr, rhs.data_ptr);
		}

		friend constexpr bool operator!=(const BasicStringView& lhs, const BasicStringView& rhs) {
			return !(lhs == rhs);
		}

	   private:
		const T* data_ptr;
		SizeType len;
	};

	template <class T, class Traits = CharTraits<T>, class Alloc = StdAllocator<T>>
	struct BasicString {
		using CharType = T;
		using SizeType = size_t;

		inline BasicString() = default;

		inline BasicString(const T* cstr) {
			ELFLDR_VERIFY(cstr != nullptr);
			CopyFromCString(cstr);
		}

		inline BasicString(const T* mem, int length) {
			ELFLDR_VERIFY(mem != nullptr);

			// TODO: maybe some interning.
			// 32 chars max intern, before it becomes an allocation.
			Resize(length);
			// memcpy(&memory[0], &mem[0], length * sizeof(T));
			Traits::Copy(&mem[0], &memory[0], length);
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
			Traits::Copy(&source.memory[0], &memory[0], source.len);
		}

		inline BasicString& operator=(const BasicString& copy) {
			if(this == &copy)
				return *this;

			// call copy constructor (for ease of implementation)
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

		inline T& operator[](SizeType index) {
			return memory[index];
		}

		inline const T& operator[](SizeType index) const {
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
					Traits::Copy(&old[0], &memory[0], len);

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
			return !Traits::Compare(lhs.data(), rhs.data());
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

			const auto clen = Traits::Length(cstr);

			Resize(clen);
			memcpy(&memory[0], &cstr[0], clen * sizeof(T));
		}

		T* memory { nullptr };
		SizeType len {};
		Alloc alloc;
	};

	// Hash<T> specializations for string stuff,
	// this will automatically work with any new
	// instantiations of BasicString or BasicStringView,
	// respecting custom Traits implementations as well.

	template <class CharT, template<class> class Traits, class Allocator>
	struct Hash<BasicString<CharT, Traits<CharT>, Allocator>> {
		inline static uint32_t hash(const BasicString<CharT, Traits<CharT>>& str) {
			return detail::fnv1a_hash(reinterpret_cast<const void*>(str.c_str()), str.length() * sizeof(CharT), 0);
		}
	};

	template <class CharT, template<class> class Traits>
	struct Hash<BasicStringView<CharT, Traits<CharT>>> {
		inline static uint32_t hash(const BasicStringView<CharT, Traits<CharT>>& str) {
			return detail::fnv1a_hash(reinterpret_cast<const void*>(str.Data()), str.Length() * sizeof(CharT), 0);
		}
	};

	using String = BasicString<char>;
	using StringView = BasicStringView<char>;

	// Tell compiler that we explicitly instantiate
	// these instations of BasicString, so the compiler
	// doesn't do extra work + linker doesn't have
	// to do it either.
	extern template struct BasicString<char>;
	extern template struct BasicStringView<char>;

	// safe for UTF-8 (?)
	// using U8String = BasicString<char8_t>;
	// using U8StringView = BasicStringView<char8_t>;

	// String Algorithms:

	template<class CharT, template<class> class Traits>
	constexpr bool StrCaseMatch(BasicStringView<CharT, Traits<CharT>> sv, BasicStringView<CharT, Traits<CharT>> sv2) {
		if(sv.Length() != sv2.Length())
			return false; // Quick shortcut

		for(StringView::SizeType i = 0; i < sv.Length(); ++i)
			if(Traits<CharT>::ToLower(sv[i]) != Traits<CharT>::ToLower(sv2[i]))
				return false; // not matching

		return true;
	}

	template<class CharT, template<class> class Traits>
	constexpr bool StrMatch(BasicStringView<CharT, Traits<CharT>> sv, BasicStringView<CharT, Traits<CharT>> sv2) {
		if(sv.Length() != sv2.Length())
			return false; // Quick shortcut

		for(StringView::SizeType i = 0; i < sv.Length(); ++i)
			if(sv[i] != sv2[i])
				return false; // not matching

		return true;
	}


} // namespace elfldr

#endif