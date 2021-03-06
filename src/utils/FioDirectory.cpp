/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <utils/FioDirectory.h>

namespace elfldr::util {

	FioDirectory::FioDirectory(mlstd::StringView path) {
		Open(path);
	}

	FioDirectory::~FioDirectory() {
		if(Good())
			fioDclose(fd);
	}

	bool FioDirectory::Open(mlstd::StringView path) {
		fd = fioDopen(path.CStr());
		if(!Good())
			return false;

		return true;
	}

	bool FioDirectory::Good() const {
		return fd > 0;
	}

	FioDirectory::operator bool() const {
		return Good();
	}

}