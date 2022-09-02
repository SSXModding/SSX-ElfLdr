/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_FIODIRECTORY_H
#define ELFLDR_FIODIRECTORY_H

#include <fileio.h>
#include <mlstd/String.h>

namespace elfldr::util {

#define ELFLDR_FIO_ISDIR(entry) (((entry).stat.mode & 0x20))
#define ELFLDR_FIO_ISREG(entry) (!((entry).stat.mode & 0x10))

	/**
	 * A safe wrapper over FIO directory iteration.
	 */
	struct FioDirectory {
		FioDirectory() = default;

		/**
		 * Shorthand constructor to do Open(path).
		 * \param[in] path Directory to open and iterate.
		 */
		explicit FioDirectory(mlstd::StringView path);

		/**
		 * Initialize this FioDirectory with an existing file descriptor.
		 *
		 * \param[in] fd An existing fd returned by fioDopen().
		 */
		explicit FioDirectory(int fd)
			: fd(fd) {
		}

		~FioDirectory();

		/**
		 * Open a directory.
		 *
		 * \param[in] path Directory to open and iterate.
		 * \return True if open succeeded, false if not.
		 */
		bool Open(mlstd::StringView path);

		/**
		 * Is the FioDirectory opened successfully?
		 * \return True if good, false otherwise.
		 */
		bool Good() const;

		/**
		 * Helper conversion operator
		 * \see FioDirectory::Ok()
		 */
		operator bool() const;

		/**
		 * Iterate through the currently open directory.
		 *
		 * \tparam DirectoryIterator A function object, ideally bool(io_dirent_t& ent). Return false to stop iteration
		 * \param[i] callback The iteration callback
		 */
		template <class DirectoryIterator>
		void Iterate(DirectoryIterator&& callback) const {
			if(!Good())
				return;
			io_dirent_t dirent{};

			while(fioDread(fd, &dirent)) {
				if(!callback(dirent))
					return;
			}
		}

	   private:
		int fd { -1 };
	};

} // namespace elfldr::util

#endif // ELFLDR_FIODIRECTORY_H
