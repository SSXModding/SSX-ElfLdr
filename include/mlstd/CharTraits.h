/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_CHARTRAITS_H
#define ELFLDR_CHARTRAITS_H

#include <mlstd/Assert.h>
#include <stddef.h>
#include <string.h>

namespace mlstd {

	/**
	 * Basic character traits.
	 */
	template <class CharT>
	struct CharTraits {
		// inline static size_t Length(const CharT* __restrict);
		// inline static void Copy(const CharT* __restrict, CharT* __restrict, size_t);
		// inline static int Compare(const CharT* __restrict, const CharT* __restrict);
		// inline static CharT ToLower(CharT c);
	};

	/**
	 * Implementation of CharTraits<> for char.
	 */
	template <>
	struct CharTraits<char> {
		inline static size_t Length(const char* __restrict str) {
			if(str == nullptr)
				return 0;
			return strlen(str);
		}

		inline static void Copy(const char* __restrict src, char* __restrict dest, size_t length) {
			memcpy(dest, src, length * sizeof(char));
		}

		inline static int Compare(const char* __restrict str1, const char* __restrict str2) {
			// NB: It is impossible for str1 == str2,
			// due to the fact that we use __restrict here.

			// Nevertheless, we do need to assert for some idiot-cases.
			ELFLDR_ASSERT(str1 != nullptr);
			ELFLDR_ASSERT(str2 != nullptr);
			return strcmp(str1, str2);
		}

		inline static char ToLower(char c) {
			if(c >= 'A' && c <= 'Z')
				return c + 32;
			else
				return c;
		}
	};

	// TODO: implementation of CharTraits for UTF-8,
	// 	so we can switch to utf-8 safe code (where we're not touching Sony libs/such)

} // namespace elfldr

#endif // ELFLDR_CHARTRAITS_H
