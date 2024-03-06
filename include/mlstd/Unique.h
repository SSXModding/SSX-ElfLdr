/**
 * SSX-Elfldr
 *
 * (C) 2021-2024 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <mlstd/Allocator.h>

namespace mlstd {

	/// A unique smart pointer. Cannot be copied, only moved.
	template <class T, Deleter TDeleter = StdDeleter<T>>
	struct Unique {
		using Pointer = T*;

		constexpr Unique() = default;

		constexpr Unique(Pointer ptr)
			: ptr(ptr) {
		}

		Unique(const Unique&) = delete;
		Unique& operator=(const Unique&) = delete;

		Unique(Unique&& move) {
			ptr = move.ptr;
			move.ptr = nullptr;
		}

		Unique& operator=(Unique&& move) {
			if(&move == &this)
				return *this;

			ptr = move.ptr;
			move.ptr = nullptr;
		}

		~Unique() {
			if(ptr)
				deleter(ptr);
		}

		Pointer Get() const {
			return ptr;
		}

		operator T*() const {
			return ptr;
		}

		Pointer operator->() const {
			return ptr;
		}

		// addrof operator (laterz;)

	   private:
		[[no_unique_address]] TDeleter deleter;
		Pointer ptr { nullptr };
	};

	template <class T, class... Args>
	auto MakeUnique(Args&&... args) {
		return Unique<T, StdDeleter<T>> { new T(static_cast<Args&&>(args)...) };
	}

} // namespace mlstd