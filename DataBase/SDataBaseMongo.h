//
// SDataBaseMongo.h
//

#ifndef SDATABASEMONGO_H
#define SDATABASEMONGO_H

#include "SDataBase.h" 

namespace mongo 
{
	class DBClientConnection;
};

namespace Sung
{

	class DataBaseMongo : public DataBase 
	{
	public:
		DataBaseMongo();
		virtual ~DataBaseMongo();

		virtual bool Open ();
		virtual bool Close ();
		virtual bool Ping ();

		virtual bool VerifyPass (const std::string &nickName,
			const std::string &password);
		virtual bool IsAccountConnected (const std::string &nickName);
		virtual void ChangePlayerState (int ID, bool online);
		virtual void ChangeAccountState (int ID, bool online);
		virtual int GetAccountIDByName (const std::string &name);
		virtual int GetPersonIDByNickName (const std::string &nickName);
		virtual EngineEvent GetCharactersList (int acctID);
		virtual void CreateCharacter (int acctID, const std::string &nickName);
		virtual void DeleteCharacter (int ID);
		virtual Player GetPlayer (int ID, bool &intern, int acctID);
		virtual NPC GetNPC (int ID);
		virtual void MajPos (float pPosX, float pPosY, float pPosZ, float pOW,
			float pOX, float pOY, float pOZ, int pId);
		virtual EngineEvent GetPlayerPos (int ID);
		virtual EngineEvent GetNPCPos (int ID);
		virtual std::vector<EngineEvent> NPCInfo(int mapID);

	private:
		mongo::DBClientConnection *mConnection;
		bool mIsDBOk;
	};

}

#endif