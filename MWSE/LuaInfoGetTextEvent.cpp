#include "LuaInfoGetTextEvent.h"

#include "LuaManager.h"
#include "LuaUtil.h"

#include "TES3DialogueInfo.h"

namespace mwse {
	namespace lua {
		namespace event {
			InfoGetTextEvent::InfoGetTextEvent(TES3::DialogueInfo * info) :
				ObjectFilteredEvent("infoGetText", info),
				m_DialogueInfo(info)
			{

			}

			sol::table InfoGetTextEvent::createEventTable() {
				auto stateHandle = LuaManager::getInstance().getThreadSafeStateHandle();
				sol::state& state = stateHandle.state;
				sol::table eventData = state.create_table();

				eventData["info"] = makeLuaObject(m_DialogueInfo);
				eventData["loadOriginalText"] = [](sol::table self) {
					setEventEnabled(false);
					std::string text = self.get<TES3::DialogueInfo*>("info")->getText();
					setEventEnabled(true);
					return std::move(text);
				};

				return eventData;
			}

			bool InfoGetTextEvent::m_EventEnabled = false;
		}
	}
}
