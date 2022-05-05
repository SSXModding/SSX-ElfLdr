/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <utils/FioDirectory.h>

namespace elfldr::util {

	FioDirectory::FioDirectory(StringView path) {
		Open(path);
	}

	FioDirectory::~FioDirectory() {
		if(Ok())
			fioDclose(fd);
	}

	bool FioDirectory::Open(StringView path) {
		fd = fioDopen(path.CStr());
		if(!Ok())
			return false;

		return true;
	}

	bool FioDirectory::Ok() const {
		return fd != -1;
	}

	FioDirectory::operator bool() const {
		return Ok();
	}

}