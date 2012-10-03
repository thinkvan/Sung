//
// SCharacter.cpp
//

#include "SCharacter.h"
#include <assert.h>
using namespace Sung;

//----------------------------------------------------------------------------
Character::Character() 
{
}
//----------------------------------------------------------------------------
Character::Character (int ID) 
{
	mID = ID;
}
//----------------------------------------------------------------------------
Character::~Character() 
{
}
//----------------------------------------------------------------------------
void Character::SetModel (const std::string &model) 
{
	mModel = model;
}
//----------------------------------------------------------------------------
void Character::SetNickName (const std::string &nickName)
{
	mNickName = nickName;
}
//----------------------------------------------------------------------------
void Character::SetLevel (int level)
{
	mLevel = level;
}
//----------------------------------------------------------------------------
void Character::SetMapID(int mapID) 
{
	mMapID = mapID;
}
//----------------------------------------------------------------------------
void Character::SetPV (int pv) 
{
	mPV = pv;
}
//----------------------------------------------------------------------------
void Character::SetPVMax (int pvMax) 
{
	mPVMax = pvMax;
}
//----------------------------------------------------------------------------
void Character::SetPosX (float posX)
{
	mPosX = posX;
}
//----------------------------------------------------------------------------
void Character::SetPosY (float posY) 
{
	mPosY = posY;
}
//----------------------------------------------------------------------------
void Character::SetPosZ (float posZ) 
{
	mPosZ = posZ;
}
//----------------------------------------------------------------------------
void Character::SetOW (float ow)
{
	mOW = ow;
}
//----------------------------------------------------------------------------
void Character::SetOX (float ox)
{
	mOX = ox;
}
//----------------------------------------------------------------------------
void Character::SetOY (float oy) 
{
	mOY = oy;
}
//----------------------------------------------------------------------------
void Character::SetOZ (float oz) 
{
	mOZ = oz;
}
//----------------------------------------------------------------------------
void Character::Move (const EngineEvent& ent) 
{
	std::map<std::string, float>::const_iterator posX = ent.mfData.find("POS_X");
	std::map<std::string, float>::const_iterator posY = ent.mfData.find("POS_Y");
	std::map<std::string, float>::const_iterator posZ = ent.mfData.find("POS_Z");
	std::map<std::string, float>::const_iterator ow = ent.mfData.find("O_W");
	std::map<std::string, float>::const_iterator ox = ent.mfData.find("O_X");
	std::map<std::string, float>::const_iterator oy = ent.mfData.find("O_Y");
	std::map<std::string, float>::const_iterator oz = ent.mfData.find("O_Z");

	assert(posX != ent.mfData.end() && posY != ent.mfData.end() && posZ != ent.mfData.end());
	assert(ow != ent.mfData.end() && ox != ent.mfData.end() && oy != ent.mfData.end() && oz != ent.mfData.end());

	mPosX = posX->second;
	mPosY = posY->second;
	mPosZ = posZ->second;
	mOW = ow->second;
	mOX = ox->second;
	mOY = oy->second;
	mOZ = oz->second;
}
//----------------------------------------------------------------------------
void Character::Move(float posX, float posY, float posZ, 
					 float ow, float ox, float oy, float oz) 
{
	mPosX = posX;
	mPosY = posY;
	mPosZ = posZ;
	mOW = ow;
	mOX = ox;
	mOY = oy;
	mOZ = oz;
}
//----------------------------------------------------------------------------
EngineEvent Character::GetPos() const
{
	EngineEvent ent;

	ent.mType = EngineEvent::NPC_POS;

	ent.mfData["POS_X"] = mPosX;
	ent.mfData["POS_Y"] = mPosY;
	ent.mfData["POS_Z"] = mPosZ;
	ent.mfData["O_W"] = mOW;
	ent.mfData["O_X"] = mOX;
	ent.mfData["O_Y"] = mOY;
	ent.mfData["O_Z"] = mOZ;

	return ent;
}
//----------------------------------------------------------------------------