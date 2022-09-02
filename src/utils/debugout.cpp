/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <utils/Utils.h>

#include <stdio.h>
#include <string.h>

#include <utils/FioFile.h>

#ifdef ERL
	#include <sdk/GameApi.h>
#endif

// internal symbol from MLSTD
extern "C" int mlstd_printf(const char* __restrict format, ...);

namespace elfldr::util {

	// Some macro (:() helpers
	// to make this a bit less garbage
#define LITERAL_STRLEN(lit) (sizeof(lit) - 1)
#define LITERAL_STRCPY(dst, lit) __builtin_memcpy(dst, lit, LITERAL_STRLEN(lit))
#define VSNPRINTF_OFFSET(buf, size, offset) __builtin_vsnprintf(&(buf)[offset], ((size) - (offset)), format, val)

	FioFile logFile;

	void DebugInit() {
		logFile.Open("host:modloader.log", FIO_O_CREAT | FIO_O_APPEND | FIO_O_RDWR);
	}

	// This code is messy since it needs to only use gcc builtins
	// to work across erl/elf boundaries, alongside some Platform Soup
	// to use bx::printf for the ERL.
	//
	// I am so sorry.
	// please don't hate me.

	void DebugOut(const char* format, ...) {
		char buf[256] {};

		__builtin_va_list val;
		__builtin_va_start(val, format);

#define DEBUGOUT_PREFIX "[Ml] "

		LITERAL_STRCPY(&buf[0], DEBUGOUT_PREFIX);
		VSNPRINTF_OFFSET(buf, sizeof(buf), LITERAL_STRLEN(DEBUGOUT_PREFIX));

#ifndef ERL
		// use nano newlib puts() where we can
		printf("%s\n", buf);
		//mlstd_printf("%s\n", buf);

		// logfile
		if(logFile.Good()) {
			logFile.WriteLine(buf);
			fioSync(FIO_WAIT, nullptr);
		}

#else
		// I could *probably* search through the binary for puts(),
		// but this is fine (and just as safe).
		bx::printf("%s\n", buf);
#endif
		__builtin_va_end(val);
	}

	void DebugClose() {
		if(logFile.Good())
			logFile.Close();
	}

} // namespace elfldr::util