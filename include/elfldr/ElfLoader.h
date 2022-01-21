/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLOADER_H
#define ELFLOADER_H

namespace elfldr {

	/**
	 * ELF loader class.
	 */
	struct ElfLoader {
		/**
		 * Load an ELF file from the given path.
		 *
		 * \param[in] inputPath Input path.
		 * \returns True if successful, false otherwise
		 */
		bool LoadElf(const char* inputPath);

		/**
		 * Execute loaded ELF file.
		 *
		 * \param[in] argc Argc to send to ELF.
		 * \param[in] argv Argv to send to ELF.
		 */
		void ExecElf(int argc, char** argv);
	};

	/**
	 * Flush all caches. Used to save ps2sdk includes
	 */
	void FlushCaches();

	/**
	 * Init loader services, reset IOP and load modules, and init libcdvd
	 */
	void InitLoader();

	/**
	 * Reset the IOP.
	 */
	void ResetIOP();
} // namespace elfldr

#endif // ELFLOADER_H