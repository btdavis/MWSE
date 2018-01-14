/************************************************************************
	
	xAddItem.cpp - Copyright (c) 2008 The MWSE Project
	http://www.sourceforge.net/projects/mwse

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

**************************************************************************/

#include "VMExecuteInterface.h"
#include "Stack.h"
#include "InstructionInterface.h"
#include "TES3Util.h"
#include "mwAdapter.h"
#include "VirtualMachine.h"
#include "ScriptUtil.h"

using namespace mwse;

namespace mwse
{
	class xAddItem : mwse::InstructionInterface_t
	{
	public:
		xAddItem();
		virtual float execute(VMExecuteInterface &virtualMachine);
		virtual void loadParameters(VMExecuteInterface &virtualMachine);
	};

	static const mwse::mwOpcode_t xAddItemOpcode = 0x3C28;
	static xAddItem xAddItemInstance;

	xAddItem::xAddItem() : mwse::InstructionInterface_t(xAddItemOpcode) {}

	void xAddItem::loadParameters(mwse::VMExecuteInterface &virtualMachine) {
	}

	float xAddItem::execute(mwse::VMExecuteInterface &virtualMachine)
	{
		// Get parameters.
		mwseString_t id = virtualMachine.getString(mwse::Stack::getInstance().popLong());
		mwLong_t count = mwse::Stack::getInstance().popLong();

		// Get other context information for AddItem.
		SCPTRecord_t* script = &virtualMachine.getScript();
		REFRRecord_t* reference = virtualMachine.getReference();
		TES3DefaultTemplate_t* itemTemplate = virtualMachine.getTemplate(id.c_str());

		// Call the original function.
		mwse::mwscript::AddItem(script, reference, itemTemplate, count);

		return 0.0f;
	}
}