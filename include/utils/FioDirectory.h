/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_FIODIRECTORY_H
#define ELFLDR_FIODIRECTORY_H

#include <runtime/String.h>
#include <fileio.h>

namespace elfldr::util {


	// this is a GIANT hack but the Newlib constants don't work,
	// so we have to define our own here.

#define ELFLDR_FIO_ISDIR(entry) (((entry).stat.mode & 0b00000001))
#define ELFLDR_FIO_ISREG(entry) (!((entry).stat.mode & 0b00000001))

	struct FioDirectory {

		FioDirectory() = default;

		explicit FioDirectory(StringView);

		bool Open(StringView path);

		~FioDirectory();

		bool Ok() const;
		operator bool() const;

		template<class DirectoryIterator>
		void Iterate(DirectoryIterator&& callback) const {
			if(!Ok())
				return;

			io_dirent_t dirent;

			while(fioDread(fd, &dirent))
				if(!callback(dirent))
					return;
		}

	   private:
		int fd{-1};
	};

}

#endif // ELFLDR_FIODIRECTORY_H
