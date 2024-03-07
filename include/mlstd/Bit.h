/**
 * SSX-Elfldr
 *
 * (C) 2021-2024 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mlstd/TypeTraits.h>

namespace mlstd {
	namespace impl {

		enum class Endian : int {
			Little = __ORDER_LITTLE_ENDIAN__,
			Big = __ORDER_BIG_ENDIAN__,
			Native = __BYTE_ORDER__
		};

        /// Concept for ensuring bit castability
		template <class Dest, class Src>
		concept BitCastable = requires {
			sizeof(Dest) == sizeof(Src);
		} && IsTrivallyCopyableV<Dest> && IsTrivallyCopyableV<Src>;

		template <class Dest, class Src>
		constexpr Dest BitCast(Src src)
			requires(BitCastable<Dest, Src>)
		{
			return __builtin_bit_cast(Dest, src);
		}
	} // namespace impl

	using impl::BitCast;
	using impl::Endian;

} // namespace mlstd