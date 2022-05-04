/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#ifndef ELFLDR_VERSIONPROBE_H
#define ELFLDR_VERSIONPROBE_H

namespace elfldr {

	/**
	 * Attempt to autodetect and fill in the game version data,
	 * from where ElfLdr was placed.
	 *
	 * \return Nothing.
	 * 		   This function will additionally halt if the PCSX2 version is too old for
	 *		   auto-detection to function properly.
	 */
	void AutodetectGameVersion();

} // namespace elfldr

#endif // ELFLDR_VERSIONPROBE_H
