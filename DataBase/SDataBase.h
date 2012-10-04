//
// SDataBase.h
//

#ifndef SDATABASE_H
#define SDATABASE_H

#include <string>
#include <vector>
#include "SEngineEvent.h"
#include "SPlayer.h"
#include "SNPC.h"

namespace Sung
{

	class DataBase
	{
	public:
		virtual ~DataBase () { Close(); }

		virtual bool Open () { return true; }
		virtual bool Close () { return true; }
		virtual bool Ping () = 0;

		virtual bool VerifyPass (const std::string &nickName, 
			const std::string &pass) = 0;
		virtual bool IsAccountConnected (const std::string &nickName) = 0;
		virtual void ChangePlayerState (int ID, bool online) = 0;
		virtual void ChangeAccountState (int ID, bool online) = 0;
		virtual int GetAccountIDByName (const std::string &name) = 0;
		virtual int GetPersonIDByNickName (const std::string &nickName) = 0;
		virtual EngineEvent GetCharactersList (int acctID) = 0;
		virtual void CreateCharacter (int acctID, const std::string &nickName) = 0;
		virtual void DeleteCharacter (int ID) = 0;
		virtual Player GetPlayer (int ID, bool &internal, int acctID) = 0;
		virtual NPC GetNPC (int ID) = 0;
		virtual void MajPos (float pPosX, float pPosY, float pPosZ, float pOW, float pOX, float pOY, float pOZ, int pId) = 0;
		virtual EngineEvent GetPlayerPos (int ID) = 0;
		virtual EngineEvent GetNPCPos (int ID) = 0;
		virtual std::vector<EngineEvent> NPCInfo(int mapID) = 0;
	};

}

#endif