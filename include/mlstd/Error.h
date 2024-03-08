/**
 * SSX-Elfldr
 *
 * (C) 2021-2024 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * SPDX-License-Identifier: MIT
 */
 
#pragma once

#include <mlstd/TypeTraits.h>

namespace mlstd {

    // N.B: I don't really want to implement it like this
    // because blegh.
	//template <class T>
	//struct IsErrorCode : FalseType {};

	enum class Errc : int {};

	// An error category.
	struct ErrorCategory {
		virtual ~ErrorCategory() = default;
		virtual const char* Name() const = 0;
		virtual const char* ToString(Errc err) const = 0;
	};

    /// A light-weight holder for an error code.
	struct ErrorCode {
		template <class T>
		constexpr ErrorCode(const ErrorCategory& category, T code)
			: category(&category), errc(static_cast<Errc>(code)) {
		}

        constexpr Errc Code() const {
            return errc;
        }

        constexpr const ErrorCategory& Category() const {
            return *category;
        }

        constexpr const char* ToString() const {
            return category->ToString(errc);
        }

	   private:
		const ErrorCategory* category;
		Errc errc {};
	};

    enum class GenericError : int {
        Ok = 0,
        UnknownError,
        NoMemory,
    };

    const ErrorCategory& GenericCategory();

    /// Base overload. Other components can provide a helper ADL override with their error code
    ErrorCode MakeErrc(GenericError e);

} // namespace mlstd