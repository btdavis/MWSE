#pragma once

#include "TES3Defines.h"

#include "TES3Object.h"

namespace TES3 {
	namespace SoundType {
		enum SoundType {
			LeftFoot = 0x0,
			RightFoot = 0x1,
			SwimLeft = 0x2,
			SwimRight = 0x3,
			Moan = 0x4,
			Roar = 0x5,
			Scream = 0x6,
			Land = 0x7,
		};
	}

	struct SoundGenerator : BaseObject {
		char name[32];
		int unknown_0x30;
		Sound * sound;
		SoundType::SoundType soundType;
	};
	static_assert(sizeof(SoundGenerator) == 0x3C, "TES3::SoundGenerator failed size validation");
}
