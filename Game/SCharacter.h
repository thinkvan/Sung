//
// SCharacter.h
//

#ifndef CHARACTER_H
#define CHARACTER_H

#include <string>
#include "SEngineEvent.h"

namespace Sung
{

	class Character 
	{
	public:
		Character ();
		Character (int ID);
		virtual ~Character ();

		inline int GetLevel() const { return mLevel; }
		inline int GetPV() const { return mPV; }
		inline int GetPVMax() const { return mPVMax; }

		inline int GetMapID() const { return mMapID; }
		inline float GetPosX() const { return mPosX; }
		inline float GetPosY() const { return mPosY; }
		inline float GetPosZ() const { return mPosZ; }

		inline float GetOW() const { return mOW; }
		inline float GetOX() const { return mOX; }
		inline float GetOY() const { return mOY; }
		inline float GetOZ() const { return mOZ; }

		inline int GetID() const { return mID; }
		inline std::string GetModel() const { return mModel; }
		inline std::string GetNickName() const { return mNickName; }
		EngineEvent GetPos() const;

		void Move (const EngineEvent &ent);
		void Move (float pPosX, float pPosY, float pPosZ, float pOW, float pOX, float pOY, float pOZ);

		void SetMapID (int mapID);
		void SetLevel (int level);
		void SetModel (const std::string &model);
		void SetNickName (const std::string &nickName);
		void SetPV (int pv);
		void SetPVMax (int pvMax);
		void SetPosX (float posX);
		void SetPosY (float posY);
		void SetPosZ (float posZ);
		void SetOW (float OW);
		void SetOX (float OX);
		void SetOY (float OY);
		void SetOZ (float OZ);

	protected:
		int mID;
		std::string mModel;
		std::string mNickName;

		int mLevel;
		int mMapID;
		float mPosX;
		float mPosY;
		float mPosZ;
		float mOW;
		float mOX;
		float mOY;
		float mOZ;
		float mVX;
		float mVY;
		float mVZ;

		int mPV;
		int mPVMax;
	};

}

#endif
