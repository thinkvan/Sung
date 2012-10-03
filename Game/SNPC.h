//
// SNPC.h
//

#ifndef SNPC_H
#define SNPC_H

#include <string>
#include "SCharacter.h"
#include "SEngineEvent.h"
#include "SItem.h"

namespace Sung
{

	class NPC : public Character
	{
	public:
		NPC ();
		NPC (int ID);
		NPC (const NPC &npc);
		~NPC ();

		template<class Archive>
		void serialize (Archive& ar, const unsigned int) 
		{
			ar & mLevel;
			ar & mMapID;
			ar & mPosX;
			ar & mPosY;
			ar & mPosZ;
			ar & mOW;
			ar & mOX;
			ar & mOY;
			ar & mOZ;
			ar & mID;
			ar & mModel;
			ar & mNickName;
			ar & mPV;
			ar & mPVMax;
		}
	};

}

#endif