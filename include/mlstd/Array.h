/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef MLSTD_ARRAY_H
#define MLSTD_ARRAY_H

#include <stddef.h>

namespace mlstd {

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

} // namespace mlstd

#endif // MLSTD_
