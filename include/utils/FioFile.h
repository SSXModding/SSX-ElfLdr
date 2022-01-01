//
// Created by lily on 1/1/22.
//

#ifndef ELFLDR_FIOFILE_H
#define ELFLDR_FIOFILE_H

#include <fileio.h>

namespace elfldr::util {

	// FIXME: This should definitely be moved into the utils library.

	/**
	 * Helper class for using PS2SDK FIO in a way which isn't garbage.
	 * Automatically closes, provides sane state management, all that.
	 */
	struct FioFile {
		/**
		 * Default constructor.
		 * This state is invalid, and you'll need to use
		 * FioFile::Open() to make it valid.
		 */
		FioFile() = default;

		/**
		 * Initalize this FioFile with an existing file descriptor.
		 *
		 * \param[in] fd An existing fd returned by fio* functions
		 */
		FioFile(int fd)
			: fd(fd) {
		}

		~FioFile();

		bool Good() const;

		operator bool() const;

		/**
		 * Close the file.
		 */
		void Close();

		/**
		 * Read some bytes from the file.
		 */
		int Read(void* buffer, size_t length);

		/**
		 * Write some bytes to the file.
		 */
		int Write(void* buffer, size_t length);

		/**
		 * Seek somewhere in the file.
		 */
		int Seek(int offset, int whence);

		/**
		 * Tell the current position of the file.
		 */
		int Tell();

		/**
		 * Convinence function to return the size of the file.
		 * Only use this for real files you baka!!!!!!!!!
		 */
		int Size();

		void Open(const char* path, int openflags);

	   private:
		/**
		 * The file descriptor.
		 */
		int fd { -1 };
	};

} // namespace elfldr::util

#endif // ELFLDR_FIOFILE_H
