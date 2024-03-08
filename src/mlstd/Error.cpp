/**
 * SSX-Elfldr
 *
 * (C) 2021-2024 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * SPDX-License-Identifier: MIT
 */
 
#include <mlstd/Error.h>

namespace mlstd {

	struct GenericCategoryImpl : public ErrorCategory {
		const char* Name() const override {
			return "generic";
		}

		const char* ToString(Errc errc) const override {
			using enum GenericError;
			switch(static_cast<GenericError>(errc)) {
				case Ok:
					return "no error";
				case UnknownError:
					return "unknown (possibly fatal) error";
				case NoMemory:
					return "out of memory to perform task";
				default:
					return "unknown error";
			}
		}
	};

	const ErrorCategory& GenericCategory() {
		static GenericCategoryImpl category;
		return category;
	}

} // namespace mlstd