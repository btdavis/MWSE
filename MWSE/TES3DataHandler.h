#pragma once

#include "TES3Object.h"
#include "TES3Collections.h"
#include "TES3MagicEffect.h"
#include "TES3Skill.h"

namespace TES3 {
	struct RecordLists {
		struct UnknownStruct {
			void * vTable; // 0x00
			long unknown_0x04;
			void * unknown_0x08;
			void * unknown_0x0C;
			void * unknown_0x10;
			long unknown_0x14;
			long unknown_0x18;
			long unknown_0x1C;
			long unknown_0x20;
			long unknown_0x24;
			long unknown_0x28;
			void * unknown_0x2C;
		};
		long unknown_0x00;
		long unknown_0x04; // always 0?
		BaseObject * unknown_0x08; // Points to info about the last loaded save?
		LinkedList<Object> * list; // 0x0C
		LinkedList<Spell> * spellsList; // 0x10
		void * unknown_0x14;
		GameSetting ** GMSTs; // 0x18 // Pointer to array of GMST pointers.
		UnknownStruct * unknown5[12]; // 0x1C
		Skill skills[27]; // 0x4C
		MagicEffect magicEffects[143]; // 0x05C8
	};
	static_assert(sizeof(RecordLists::UnknownStruct) == 0x30, "TES3::RecordList::UnknownStruct failed size validation");
	static_assert(sizeof(RecordLists) == 0x9DB8, "TES3::RecordList failed size validation");

	namespace CellGrid {
		enum CellGrid {
			NorthWest = 0,
			North = 1,
			NorthEast = 2,
			West = 3,
			Center = 4,
			East = 5,
			SouthWest = 6,
			South = 7,
			SouthEast = 8
		};
	}

	struct DataHandler {
		RecordLists * recordLists; // 0x0
		CellExteriorData* exteriorCellData[9]; // 0x4
		void * unknown_0x28[25]; // 0x28
		void * worldObjectRoot; // 0x8C
		void * worldPickObjectRoot; // 0x90
		void * worldLandscapeRoot; // 0x94
		char unknown_0x98;
		char unknown_0x99;
		char unknown_0x9A;
		char unknown_0x9B;
		char unknown_0x9C;
		char unknown_0x9D;
		char unknown_0x9E;
		char unknown_0x9F;
		int centralGridX; // 0xA0
		int centralGridY; // 0xA4
		int cellChanged; // 0xA8
		Cell * currentInteriorCell; // 0xAC
		void * interiorCellBuffer; // 0xB0
		void * exteriorCellBuffer; // 0xB4
		int unknown_0xB8;
		int unknown_0xBC;
		Iterator<void> collisionGrid[2304]; // 0xC0
		int unknown_0xB4C0;
		int unknown_0xB4C4;
		char unknown_0xB4C8;
		char unknown_0xB4C9;
		char unknown_0xB4CA;
		char unknown_0xB4CB;
		char unknown_0xB4CC;
		char unknown_0xB4CD;
		char unknown_0xB4CE;
		char unknown_0xB4CF;
		void * sounds; // 0xB4D0
		void * tempSounds; // 0xB4D4
		void * unknown_0xB4D8; // 0xB4D8
		char unknown_0xB4DC;
		char unknown_0xB4DD;
		char unknown_0xB4DE;
		char unknown_0xB4DF;
		char unknown_0xB4E0;
		char unknown_0xB4E1;
		char unknown_0xB4E2;
		char unknown_0xB4E3;
		char unknown_0xB4E4;
		char unknown_0xB4E5;
		char unknown_0xB4E6;
		char unknown_0xB4E7;
		char unknown_0xB4E8;
		char unknown_0xB4E9;
		char unknown_0xB4EA;
		char unknown_0xB4EB;
		int unknown_0xB4EC;
		int unknown_0xB4F0;
		int unknown_0xB4F4;
		char unknown_0xB4F8;
		char unknown_0xB4F9;
		char unknown_0xB4FA;
		char unknown_0xB4FB;
		int backgroundThreadID; // 0xB4FC
		int mainThreadID; // 0xB500
		int backgroundThread; // 0xB504
		int mainThread; // 0xB508
		char unknown_0xB50C;
		char unknown_0xB50D;
		char unknown_0xB50E;
		char unknown_0xB50F;
		char unknown_0xB510;
		unsigned char backgroundThreadRunning; // 0xB511
		char unknown_0xB512;
		char unknown_0xB513;
		int unknown_0xB514;
		char unknown_0xB518;
		char unknown_0xB519;
		char unknown_0xB51A;
		char unknown_0xB51B;
		int threadSleepTime; // 0xB51C
		int unknown_0xB520; // Time interval?
		char unknown_0xB524;
		char unknown_0xB525;
		char unknown_0xB526;
		char unknown_0xB527;
		float unknown_0xB528;
		float unknown_0xB52C;
		char unknown_0xB530;
		char unknown_0xB531;
		char unknown_0xB532;
		char unknown_0xB533;
		char unknown_0xB534;
		char unknown_0xB535;
		char unknown_0xB536;
		char unknown_0xB537;
		int criticalSection; // 0xB538
		char unknown_0xB53C;
		char unknown_0xB53D;
		char unknown_0xB53E;
		char unknown_0xB53F;
		Cell * currentCell; // 0xB540
		Cell * lastExteriorCell; // 0xB544
		char unknown_0xB548;
		char unknown_0xB549;
		char unknown_0xB54A;
		char unknown_0xB54B;
		char unknown_0xB54C;
		char unknown_0xB54D;
		char unknown_0xB54E;
		char unknown_0xB54F;
		long exteriorCellDataBufferSize; // 0xB550
		void * exteriorCellDataBuffer; // 0xB554
	};
	static_assert(sizeof(DataHandler) == 0xB558, "TES3::DataHandler failed size validation");
}