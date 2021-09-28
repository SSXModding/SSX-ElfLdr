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
	 * Init loader services, reset IOP and load modules, and init libcdvd
	 */
	void InitLoader();
	
	/**
	 * Reset the IOP.
	 */
	void ResetIOP();
}

#endif // ELFLOADER_H