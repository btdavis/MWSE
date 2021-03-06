#pragma once

#include "NIDynamicEffect.h"

#include "LuaUtil.h"

#include "NIObjectLua.h"

namespace mwse {
	namespace lua {
		// Speed-optimized binding for NI::DynamicEffect.
		template <typename T>
		void setUserdataForNIDynamicEffect(sol::simple_usertype<T>& usertypeDefinition) {
			setUserdataForNIAVObject(usertypeDefinition);

			// Basic property binding.
			usertypeDefinition.set("affectedNodes", &NI::DynamicEffect::affectedNodes);
			usertypeDefinition.set("enabled", &NI::DynamicEffect::enabled);

			// Functions exposed as properties.
			usertypeDefinition.set("type", sol::readonly_property(&NI::DynamicEffect::getType));
		}

		void bindNIDynamicEffect();
	}
}
