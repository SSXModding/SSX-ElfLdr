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
	 * Flush both I & D caches. Used to save ps2sdk includes
	 */
	void FlushCaches();

	/**
	 * Initialize loader services, reset IOP and load modules.
	 * Call this before using ElfLoader.
	 */
	void InitLoader();

	/**
	 * Reset the IOP.
	 */
	void ResetIOP();


	/**
	 * ELF loader class.
	 */
	struct ElfLoader {

		ElfLoader();

		/**
		 * Load an ELF file from the given path.
		 *
		 * \param[in] inputPath Input path.
		 * \returns True if successful, false otherwise
		 */
		bool LoadElf(const char* inputPath);

		/**
		 * Execute loaded ELF file.
		 * Only call this function if ElfLoader::LoadElf() returned true!
		 *
		 * \param[in] argc Argc to send to ELF.
		 * \param[in] argv Argv to send to ELF.
		 */
		void ExecElf(int argc, char** argv);
	};

} // namespace elfldr

#endif // ELFLOADER_H