#ifndef ELFLDR_ARRAY_H
#define ELFLDR_ARRAY_H

#include <cstddef>

namespace elfldr::util {

	/**
	 * A POD array.
	 */
	template<class T, size_t N>
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

}

#endif // ELFLDR_ARRAY_H
