/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_ARRAY_H
#define ELFLDR_ARRAY_H

#include <cstddef>

namespace elfldr::util {

	/**
	 * A POD array.
	 */
	template <class T, size_t N>
	struct Array {
		constexpr size_t Size() {
			return N;
		}

		constexpr T* Data() {
			return &_arr_notouchy[0];
		}

		constexpr const T* Data() const {
			return &_arr_notouchy[0];
		}

		// only public to keep POD contract true
		T _arr_notouchy[N];
	};

} // namespace elfldr::util

#endif // ELFLDR_ARRAY_H
