#include "NICameraLua.h"

#include "sol.hpp"

#include "LuaManager.h"
#include "LuaUtil.h"

#include "NICamera.h"
#include "NINode.h"
#include "NIRTTI.h"

#include "TES3Vectors.h"

namespace mwse {
	namespace lua {
		void bindNICamera() {
			sol::state& state = LuaManager::getInstance().getState();

			state.new_usertype<NI::Camera>("NICamera",
				// Disable construction of this type.
				"new", sol::no_constructor,

				//
				// Properties.
				//

				"runTimeTypeInformation", sol::readonly_property([](NI::Camera& self) { return self.getRunTimeTypeInformation(); }),
				"references", &NI::Camera::references,

				"name", sol::readonly_property(&NI::Camera::name),

				"flags", &NI::Camera::flags,
				"parentNode", &NI::Camera::parentNode,
				"worldBoundOrigin", &NI::Camera::worldBoundOrigin,
				"worldBoundRadius", &NI::Camera::worldBoundRadius,
				"localRotation", &NI::Camera::localRotation,
				"localTranslate", &NI::Camera::localTranslate,
				"localScale", &NI::Camera::localScale,
				"worldTransform", &NI::Camera::worldTransform,

				//
				// Methods.
				//

				"isOfType", [](NI::Camera& self, unsigned int type) { return self.isOfType((NI::RunTimeTypeInformation::RTTI)type); },
				"isInstanceOfType", [](NI::Camera& self, unsigned int type) { return self.isInstanceOfType((NI::RunTimeTypeInformation::RTTI)type); },

				"getObjectByName", [](NI::Camera& self, const char* name) { return makeLuaObject(self.getObjectByName(name)); }

			);
		}
	}
}
