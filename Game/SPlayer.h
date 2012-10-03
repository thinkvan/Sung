//
// SPlayer.h
//

#ifndef SPLAYER_H
#define SPLAYER_H

#include <string>
#include "SCharacter.h"
#include "SEngineEvent.h"
#include "SItem.h"

namespace Sung
{

	class Player : public Character 
	{
	public:
		Player ();
		Player (int ID);
		Player (const Player &player);
		~Player ();

		template<class Archive>
		void serialize (Archive &ar, const unsigned int)
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
			ar & m_pv;
			ar & m_pvMax;
		}
	};

}

#endif