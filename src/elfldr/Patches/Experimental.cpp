/**
 * SSX-Elfldr
 *
 * (C) 2021-2022 Lily/modeco80 <lily.modeco80@protonmail.ch>
 * under the terms of the MIT license.
 */

#include <erl/ErlLoader.h>
#include <runtime/Allocator.h>
#include <sdk/ErlAbi.h>
#include <sdk/GameApi.h>
#include <sdk/OGStructs.h>
#include <utils/CodeUtils.h>
#include <utils/GameVersion.h>
#include <utils/Hook.h>
#include <utils/Utils.h>

#include "../Patch.h"

namespace elfldr {

	struct ExpPatch : public ElfPatch {
		const char* GetName() const override {
			return "Experimental";
		}

		const char* GetIdentifier() const override {
			return "experimental-1";
		}

		void Apply_SSXOG(const util::GameVersionData& versionData) {
			switch(versionData.version) {
				case util::GameVersion::SSXOG_10:
					switch(versionData.region) {
						case util::GameRegion::NTSC:

							// Not using this.
							// If you want to hand patch this space in a codehook using this code as a template,
							// feel free to go right ahead, I'm not touching this space anymore..
#if 0
							// Replace the loop in cGame::UpdateNodes()
							// with a hand-written 3-instruction replacement.
							//
							// Instructions from 0x00189c24 to 0x00189c3c are completely fair game, for
							// any code you want to run during the node updating stage of cGame::Update().
							// 0x00189c24 shouldn't modify a0 or a1, however.
							//
							// Enjoy the game loop code exec possibilities..
							// (*devilish laughter*)

							// The assembly:
							//
							// addiu a0, gp, 0xFFFFBDE8 ; load gNodeManager address into a0 (this parameter) (maybe unneeded?)
							// li a1, 0x3               ; load function's first parameter into a1 (3)
							// jal 0x001864b0           ; call (linking jump) the cNodeManager function

							util::NopFill<10>(util::Ptr(0x00189c18)); // start by nopfilling

							// Put in the replacement instructions
							util::MemRefTo<uint32_t>(util::Ptr(0x00189c18)) = 0x2784bde8;
							util::MemRefTo<uint32_t>(util::Ptr(0x00189c1c)) = 0x24050003;
							util::MemRefTo<uint32_t>(util::Ptr(0x00189c20)) = 0x0c06192c;
#endif

							break;

						default:
							break;
					}
				default:
					break;
			}
		}

		void Apply() override {
			const auto& versionData = util::GetGameVersionData();

			switch(versionData.game) {
				case util::Game::SSXOG:
					Apply_SSXOG(versionData);
					break;

				default:
					break;
			}
		}
	};

	// Register the patch into the patch system
	static PatchRegistrar<ExpPatch, 0xE0> registrar;

} // namespace elfldr
