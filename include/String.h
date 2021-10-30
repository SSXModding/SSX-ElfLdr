// How long could lily go without a good string type,
// according to the commit history of Elfldr?
//
// 24 days. That's not too bad!
//
// (okaybutseriouslyIcantfuckingtakeitanymore)
//
// Unfortunately, due to the fact this uses new[]
// like no other, this currently can't be used in
// anything other than liberl. Oh well, maybe I should
// relax my zero-allocation policy a little bit.

#ifndef ELFLDR_STRING_H
#define ELFLDR_STRING_H

#include <cstdint>
#include <cstring>

namespace elfldr {

	template <class T>
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

		inline ~BasicString() {
			Resize(0);
		}

		inline const T* c_str() const {
			return memory;
		}

		inline SizeType length() const {
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
				// Destroy the buffer, if
				// we have one to destroy
				if(memory) {
					len = 0;
					delete[] memory;
				}
				return;
			}

			// save old buffer
			auto* old = memory;

			// allocate new buffer
			memory = new T[newLength + 1];
			memory[newLength] = '\0';

			if(old) {
				// copy the old buffer in
				// TODO: could probably maybe truncate for any length?
				if(len <= newLength)
					memcpy(&memory[0], &old[0], len * sizeof(T));

				// don't need it
				delete[] old;
			}
			len = newLength;
		}

		// TODO (maybe?):
		//
		// BasicString substr(SizeType pos, SizeType len) - Returns a new allocated substring of this source string

		// equality operators
		// operator== might need some work done to it.

		friend inline bool operator==(const BasicString& lhs, const BasicString& rhs) {
			// would probably need some work for introducing U8String
			return !strcmp(lhs.data(), rhs.data());
		}

		friend inline bool operator!=(const BasicString& lhs, const BasicString& rhs) {
			return !(lhs == rhs);
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
	};

	// basic string type.
	using String = BasicString<char>;

	// basic string type - safe for UTF-8 TODO
	// using U8String = BasicString<std::uint8_t>;
} // namespace elfldr

#endif