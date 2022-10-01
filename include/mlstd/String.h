/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef MLSTD_STRING_H
#define MLSTD_STRING_H

#include <mlstd/Allocator.h>
#include <mlstd/CharTraits.h>
#include <mlstd/Hash.h>

namespace mlstd {

	// TODO: Move to a separate StringView header
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

		constexpr BasicStringView() noexcept
			: data_ptr(nullptr),
			  len(0) {
		}

		// Permit the compiler to force generate a trivial
		// copy constructor.
		constexpr BasicStringView(const BasicStringView&) = default;

		constexpr BasicStringView(const T* ptr) noexcept
			: data_ptr(ptr),
			  len(strlen(ptr)) {
		}

		constexpr BasicStringView(T* ptr, SizeType len) noexcept
			: data_ptr(ptr),
			  len(len) {
		}

		[[nodiscard]] constexpr SizeType Length() const noexcept {
			return len;
		}

		constexpr const T* Data() const {
			return data_ptr;
		}

		constexpr const T* CStr() const noexcept {
			return data_ptr;
		}

		constexpr const T& operator[](SizeType index) const noexcept {
			return data_ptr[index];
		}

		friend constexpr bool operator==(const BasicStringView& lhs, const BasicStringView& rhs) noexcept {
			return !Traits::Compare(lhs.data_ptr, rhs.data_ptr);
		}

		friend constexpr bool operator!=(const BasicStringView& lhs, const BasicStringView& rhs) noexcept {
			return !(lhs == rhs);
		}

	   private:

		const T* data_ptr;
		SizeType len;
	};

	// TODO:
	//  - Clean up SSO implementation
	//		- Make class a bit less grody
	//		- Add adapters for allocating/deallocating based on SSO tag

	template <class T, class Traits = CharTraits<T>, class Alloc = StdAllocator<T>>
	struct BasicString {
		using CharType = T;
		using SizeType = size_t;

		inline BasicString() = default;

		inline BasicString(const T* cstr) noexcept {
			MLSTD_VERIFY(cstr != nullptr);
			CopyFromCString(cstr);
		}

		inline BasicString(const T* mem, int length) noexcept {
			MLSTD_VERIFY(mem != nullptr);

			Resize(length);
			Traits::Copy(&mem[0], &GetMemory()[0], length);
		}

		inline BasicString(BasicString&& move) noexcept {

			if(move.isSmall) {
				isSmall = move.isSmall;
				move.storage = storage;
			} else {
				isSmall = false;
				storage.InitAllocated();
				storage.allocated.memory = move.storage.allocated.memory;
				storage.allocated.len = move.storage.allocated.len;

				// exchange allocators (for now); should implement a trait
				// to allow allocators to tell code when to not exchange
				storage.allocated.alloc = move.storage.allocated.alloc;
			}

		}

		inline BasicString(const BasicString& source) noexcept {
			// new buffer.
			Resize(source.GetSize());
			Traits::Copy(&source.GetMemory()[0], GetMemory(), source.length());
		}

		inline BasicString& operator=(const BasicString& copy) noexcept {
			if(this == &copy)
				return *this;

			// call copy constructor (for ease of implementation)
			new(this) BasicString(copy);
			return *this;
		}

		inline BasicString& operator=(BasicString&& move) noexcept {
			return *new (this) BasicString(move);
		}

		inline ~BasicString() {
			Resize(0);
			// TODO: the above Resize(0) should be handled here, except w/ brute force
			DestroyStorage();
		}

		inline const T* c_str() const noexcept {
			return GetMemory();
		}

		[[nodiscard]] inline SizeType length() const noexcept {
			return GetSize();
		}

		inline T& operator[](SizeType index) noexcept {
			return GetMemory()[index];
		}

		inline const T& operator[](SizeType index) const noexcept {
			return GetMemory()[index];
		}

		inline T* data() noexcept {
			return GetMemory();
		}

		inline const T* data() const noexcept {
			return GetMemory();
		}

		inline void Resize(SizeType newLength) noexcept {
			if(newLength == 0) {
				// Free non-sso buffer
				if(!isSmall) {
					storage.allocated.Deallocate();
				} else {
					storage.small.Deallocate();
				}
				return;
			}

			if(newLength + 1 > SsoStorage::Small::SSO_BUFFER_SIZE) {
				// We'll need to allocate memory for this..

				// save old buffer so we can free it later
				auto* old = GetMemory();
				bool wasSmall = isSmall;

				storage.InitAllocated();

				isSmall = false;
				storage.allocated.Allocate(newLength + 1);
				GetMemory()[newLength] = '\0';

				if(old) {
					if(GetSize() <= newLength)
						Traits::Copy(&old[0], &GetMemory()[0], GetSize());

					// If the old buffer came from SSO previously, make sure
					// to handle that case so that we don't break anything
					if(!wasSmall)
						storage.allocated.alloc.Deallocate(old);
				}

			} else {
				storage.InitSmall();
				storage.small.Allocate(newLength + 1);
			}
		}

		inline BasicString substr(SizeType pos, SizeType len = -1) noexcept {
			if(pos > GetSize())
				return "";

			if(len != -1) {
				if((pos + len) > GetSize())
					return "";
			} else {
				len = 0;

				auto d = &data()[pos];

				// let's try to find a null terminator
				while(*d != '\0') {
					// If we weren't able to find the terminator, then give up,
					// and just say it's the end of the string.
					if(d == &data()[GetSize()]) {
						len = pos - GetSize();
						break;
					}

					++len;
					++d;
				}
			}

			return BasicString(&data()[pos], len);
		}

		friend inline bool operator==(const BasicString& lhs, const BasicString& rhs) noexcept {
			return !Traits::Compare(lhs.data(), rhs.data());
		}

		friend inline bool operator!=(const BasicString& lhs, const BasicString& rhs) noexcept {
			return !(lhs == rhs);
		}

		explicit operator BasicStringView<T>() noexcept {
			return BasicStringView<T>(data(), length());
		}

	   private:
		void CopyFromCString(const T* cstr) noexcept {
			if(!cstr)
				return;

			const auto clen = Traits::Length(cstr);

			Resize(clen);
			memcpy(&GetMemory()[0], &cstr[0], clen * sizeof(T));
		}

		constexpr T* GetMemory() noexcept {
			if(isSmall)
				return &storage.small.ssoMemory[0];

			return storage.allocated.memory;
		}

		constexpr const T* GetMemory() const noexcept {
			if(isSmall)
				return &storage.small.ssoMemory[0];

			return storage.allocated.memory;
		}

		constexpr SizeType GetSize() const noexcept {
			if(isSmall)
				return storage.small.len;
			return storage.allocated.len;
		}


		constexpr void DestroyStorage() noexcept {
			if(isSmall)
				storage.allocated.~Allocated();
			else
				storage.small.~Small();
		}

		/**
		 * True if SSO storage is being used.
		 */
		bool isSmall{true};

		union SsoStorage {
			~SsoStorage() = default;

			struct Allocated {
				T* memory { nullptr };
				SizeType len {};
				Alloc alloc;

				// pads the size to 16/32 bytes
				SizeType pad[2];

				void Allocate(SizeType length) {
					memory = alloc.Allocate(length + 1);
					len = length;
				}

				void Deallocate() {
					if(memory)
						alloc.Deallocate(memory);
				}
			} allocated;

			struct Small {
				constexpr static auto SSO_BUFFER_SIZE = sizeof(Allocated) - sizeof(SizeType) / sizeof(T);
				T ssoMemory[SSO_BUFFER_SIZE];
				SizeType len;

				void Allocate(SizeType length) {
					MLSTD_VERIFY(length >= SSO_BUFFER_SIZE && "Invalid Allocate() for SSO");
					len = length;
				}

				void Deallocate() {
					len = 0;
				}
			} small;

			void InitAllocated() {
				new (&allocated) Allocated;
			}

			void InitSmall() {
				new (&small) Small;
			}

			static_assert(sizeof(Small) == sizeof(Allocated), "SSO storage types deviated, fix it please");
		} storage {};

	};

	// Hash<T> specializations for string stuff,
	// this will automatically work with any new
	// instantiations of BasicString or BasicStringView,
	// respecting custom Traits implementations as well.

	template <class CharT, template <class> class Traits, class Allocator>
	struct Hash<BasicString<CharT, Traits<CharT>, Allocator>> {
		inline static uint32_t hash(const BasicString<CharT, Traits<CharT>>& str) noexcept {
			return detail::fnv1a_hash(reinterpret_cast<const void*>(str.c_str()), str.length() * sizeof(CharT), 0);
		}
	};

	template <class CharT, template <class> class Traits>
	struct Hash<BasicStringView<CharT, Traits<CharT>>> {
		inline static uint32_t hash(const BasicStringView<CharT, Traits<CharT>>& str) noexcept {
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

	// safe for UTF-8 (?)
	// using U8String = BasicString<char8_t>;
	// using U8StringView = BasicStringView<char8_t>;

	// String Algorithms:

	template <class CharT, template <class> class Traits>
	constexpr bool StrCaseMatch(BasicStringView<CharT, Traits<CharT>> sv, BasicStringView<CharT, Traits<CharT>> sv2) noexcept {
		if(sv.Length() != sv2.Length())
			return false; // Quick shortcut

		for(StringView::SizeType i = 0; i < sv.Length(); ++i)
			if(Traits<CharT>::ToLower(sv[i]) != Traits<CharT>::ToLower(sv2[i]))
				return false; // not matching

		return true;
	}

	template <class CharT, template <class> class Traits>
	constexpr bool StrMatch(BasicStringView<CharT, Traits<CharT>> sv, BasicStringView<CharT, Traits<CharT>> sv2) noexcept {
		if(sv.Length() != sv2.Length())
			return false; // Quick shortcut

		for(StringView::SizeType i = 0; i < sv.Length(); ++i)
			if(sv[i] != sv2[i])
				return false; // not matching

		return true;
	}

} // namespace mlstd

#endif // MLSTD_STRING_H