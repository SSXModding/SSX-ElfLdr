/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <mlstd/Allocator.h>
#include <mlstd/Optional.h>
#include <sdk/GameApi.h>
#include <utils/GameVersion.h>

namespace elfldr::util {

	using Init_t = void (*)();

	void SetupAllocator() {
		const auto& verData = GetGameVersionData();

		// This sets the addresses of functions we need to use

		switch(verData.game) {
			case Game::SSXOG: {
				switch(verData.version) {
					case GameVersion::SSXOG_10:

						switch(verData.region) {
							case GameRegion::NTSC:
								bx::real::MEM_init.SetFunctionAddress(0x0023b2a0);
								bx::real::initheapdebug.SetFunctionAddress(0x0018a280);

								bx::real::MEM_alloc.SetFunctionAddress(0x0023a448);
								bx::real::MEM_free.SetFunctionAddress(0x0023a998);

								// We don't really need to set this, it's just a curiosity.
								bx::printf.SetFunctionAddress(0x0018ac08);
								break;
							case GameRegion::NTSCJ:
								break;
							default:
								break;
						}
						break;

					default:
						break;
				}
			} break;

			case Game::SSXDVD:
				switch(verData.version) {
					case GameVersion::SSXDVD_10:
						switch(verData.region) {
							case GameRegion::NTSC:
								bx::real::MEM_init.SetFunctionAddress(0x002cd798);
								bx::real::initheapdebug.SetFunctionAddress(0x002cd798);

								bx::real::MEM_alloc.SetFunctionAddress(0x002ccf70);
								bx::real::MEM_free.SetFunctionAddress(0x002ccfc0);
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}
				break;

			default:
				break;
		}

		// seems like all regions and versions use the same exact params, so I guess I can wing it this time
		uintptr_t oldreal_memstart = 0x002d9440;
		int oldreal_memsize = 30432192;
		uintptr_t oldreal_unk_before_memstart = 0x002d8c20;

		// Then actually initalize the allocator.

		switch(verData.game) {
			case Game::SSXOG:
			case Game::SSXDVD:

				if(verData.game == Game::SSXDVD) {
					oldreal_memstart = 0x0040c180; // weird
					oldreal_memsize = 29177472;
					oldreal_unk_before_memstart = oldreal_memstart - 0x800;
				}

#ifndef ERL
				// Initialize the allocator
				bx::real::MEM_init(util::Ptr(oldreal_memstart), oldreal_memsize);
				bx::real::initheapdebug(oldreal_memstart, oldreal_unk_before_memstart, oldreal_memstart + oldreal_memsize);
#endif

				// clang-format off

				// Setup allocation functions
				mlstd::SetAllocationFunctions({
						[](uint32_t size) {
							return bx::real::MEM_alloc("Lily <3", size, 0x0 /* i forgor mbflags :( */);
						}, [](void* obj) {
							 bx::real::MEM_free(obj);
						}
				});

				// clang-format on

				break;

			default:
				break;
		}

		// Once we leave this function modloader can now allocate
	}

} // namespace elfldr::util