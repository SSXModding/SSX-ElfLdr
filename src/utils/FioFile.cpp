/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <mlstd/Assert.h>
#include <utils/FioFile.h>
#include <utils/Utils.h>

namespace elfldr::util {

	FioFile::~FioFile() {
		Close();
	}

	bool FioFile::Good() const {
		return fd > 0;
	}

	FioFile::operator bool() const {
		return Good();
	}

	void FioFile::Close() {
		if(Good())
			fioClose(fd);
	}

	int FioFile::Read(void* buffer, size_t length) {
		if(Good())
			return fioRead(fd, buffer, length);

		return -1;
	}

	int FioFile::Write(void* buffer, size_t length) {
		if(Good())
			return fioWrite(fd, buffer, length);

		return -1;
	}

	int FioFile::Seek(int offset, int whence) {
		ELFLDR_ASSERT(Good());
		if(Good())
			return fioLseek(fd, offset, whence);

		return -1;
	}

	int FioFile::Tell() {
		// are you kidding me? No tell?
		// whatever, this works
		if(Good())
			return fioLseek(fd, 0, FIO_SEEK_CUR);

		return -1;
	}

	void FioFile::Open(const char* path, int openflags) {
		// If there's already a file managed by this object,
		// close it to prevent fd leaks.
		if(Good())
			Close();

		fd = fioOpen(path, openflags);
	}

} // namespace elfldr::util