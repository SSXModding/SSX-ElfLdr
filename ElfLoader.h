#ifndef ELFLOADER_H
#define ELFLOADER_H

namespace elfldr {
	
	struct ElfLoader {
		
		
		// Load an ELF file. Returns false if the elf failed to load.
		bool LoadElf(const char* inputPath);
		
		// Execute ELF file.
		void ExecElf(char** argv);
		
	};
	
	void InitLoader();
	
	void ResetIOP();
}

#endif // ELFLOADER_H