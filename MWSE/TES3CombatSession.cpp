#include "TES3CombatSession.h"

#include "LuaManager.h"

#include "LuaDetermineActionEvent.h"
#include "LuaDeterminedActionEvent.h"

namespace TES3 {
	const auto TES3_CombatSession_chooseAlchemyWithEffect = reinterpret_cast<float(__thiscall*)(CombatSession*, short)>(0x538330);
	float CombatSession::chooseAlchemyWithEffect(short id) {
		return TES3_CombatSession_chooseAlchemyWithEffect(this, id);
	}

	const auto TES3_CombatSession_changeEquipment = reinterpret_cast<void(__thiscall*)(CombatSession*, EquipmentStack*)>(0x5379E0);
	void CombatSession::changeEquipment(TES3::EquipmentStack* equipmentStack) {
		TES3_CombatSession_changeEquipment(this, equipmentStack);
	}

	const auto TES3_CombatSession_determineNextAction = reinterpret_cast<void (__thiscall*)(CombatSession*)>(0x538F00);
	void CombatSession::determineNextAction() {
		auto& luaManager = mwse::lua::LuaManager::getInstance();
		if (mwse::lua::event::DetermineActionEvent::getEventEnabled()) {
			auto stateHandle = luaManager.getThreadSafeStateHandle();
			sol::table result = stateHandle.triggerEvent(new mwse::lua::event::DetermineActionEvent(this));
			if (result.valid() && result["block"] == true) {
				if (mwse::lua::event::DeterminedActionEvent::getEventEnabled()) {
					stateHandle.triggerEvent(new mwse::lua::event::DeterminedActionEvent(this));
				}
				return;
			}
		}

		TES3_CombatSession_determineNextAction(this);

		if (mwse::lua::event::DeterminedActionEvent::getEventEnabled()) {
			luaManager.getThreadSafeStateHandle().triggerEvent(new mwse::lua::event::DeterminedActionEvent(this));
		}
	}
}
