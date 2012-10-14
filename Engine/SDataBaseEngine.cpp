//
// SDataBaseEngine.cpp
//

#include <boost/archive/text_oarchive.hpp>>
#include "SDataBaseEngine.h"
#include "SDataBaseMongo.h"
#include "SConvert.h"
#include "SLog.h"

//----------------------------------------------------------------------------
#define DEBUG_DB_DISPLAY_REQUESTS
//----------------------------------------------------------------------------
DataBaseEngine::DataBaseEngine (Game *game, int logPriority, int logOutput) 
:
Engine(game, logPriority, logOutput)
{	
	mDataBase = new DataBaseMongo;

	mLog = new Log("Database", logPriority, logOutput);
	mLog->Info("Connecting to database...");

	if (!mDataBase->Open())
	{
		mLog->Error("Error while connecting to database");
	} 
	else 
	{
		mLog->Info("Connected to database"); 
	}
}
//----------------------------------------------------------------------------
DataBaseEngine::~DataBaseEngine() 
{
	bool ok = true;
	mLog->Info("Closing database...");

	try 
	{
		mDataBase->Close();
		delete mDataBase;
	}
	catch(std::exception&e) 
	{
		mLog->Error("Handled exception while closing database : " +
			std::string(e.what()));

		ok = false;
	} 
	catch(...) 
	{
		mLog->Error("Handled unknown exception while closing database");

		ok = false;
	}

	if (ok)
	{
		mLog->Info("Database successfully closed");
	}
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessEnvironmentInfo(const EngineEvent &ent)
{
	std::map<std::string, int>::const_iterator id = ent.miData.find("ID");
	std::map<std::string, int>::const_iterator map_id = ent.miData.find("MAP_ID");
	
	assert((id != ent.miData.end()) && (map_id != ent.miData.end()));
	
	SendNPCInfo(id->second, map_id->second);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessNpcProp(const EngineEvent &ent)
{
	EngineEvent e;
	std::map<std::string, int>::const_iterator npcID = 
		ent.miData.find("NPC_ID_REQ");
	std::map<std::string, int>::const_iterator id = ent.miData.find("ID");

	assert(npcID != ent.miData.end() && id != ent.miData.end());

	NPC n = GetNPCByID(npcID->second);
	
	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive << n;
	const std::string &s = archiveStream.str();
	e.mType = EngineEvent::NPC_PROP;
	e.msData["NPC_PROP"] = s;
	e.miData["ID"] = id->second;
	e.miData["NPC_ID"] = n.GetID();

	SendMessageToNet(e);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessPlayerProp(const EngineEvent& ent) 
{
	EngineEvent e;
	std::map<std::string, int>::const_iterator playerID = 
		ent.miData.find("CHARACTER_ID");
	std::map<std::string, int>::const_iterator id = 
		ent.miData.find("ID");
	std::map<std::string, int>::const_iterator acctId = 
		ent.miData.find("ACCT_ID");

	assert(playerID != ent.miData.end() && id != ent.miData.end() && 
		acctId != ent.miData.end());

	bool internal = false;
	Player p = GetPlayerByID(playerID->second, internal, acctId->second);

	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive << p;
	const std::string &s = archiveStream.str();
	e.mType = EngineEvent::PLAYER_PROP;
	e.msData["PLAYER_PROP"] = s;
	e.miData["ID"] = id->second;
	e.miData["INTERNAL"] = internal;

	// maybe remove informations if not internal ?
	SendMessageToNetwork(e);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessCharactersList(const EngineEvent &ent) 
{
	EngineEvent l;
	std::map<std::string, int>::const_iterator id = ent.miData.find("ID");
	std::map<std::string, int>::const_iterator acctId =
		ent.miData.find("ACCT_ID");

	assert(acctId != ent.miData.end() && id != ent.miData.end());

	l = mDataBase->GetCharactersList(acctId->second);
	l.miData["ID"] = id->second;

	SendMessageToNetwork(l);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessAuth (const EngineEvent &ent)
{
	EngineEvent auth;
	std::map<std::string, int>::const_iterator id = ent.miData.find("ID");
	std::map<std::string, std::string>::const_iterator acctName = 
		ent.msData.find("ACCT_NAME");
	std::map<std::string, std::string>::const_iterator pass = 
		ent.msData.find("PASS");

	assert(id != ent.miData.end() && acctName != ent.msData.end() &&
		pass != ent.msData.end());

	auth.mType = EngineEvent::AUTH;
	auth.miData["ID"] = id->second;
	auth.msData["ACCT_NAME"] = acctName->second;
	if (mDataBase->verifyPass(acctName->second, pass->second)) {
		auth.miData["ACCEPTED"] = 1;
		auth.miData["ACCT_ID"] = mDataBase->getAcctIdByName(acctName->second);
	} else
		auth.miData["ACCEPTED"] = 0;
	SendMessageToNetwork(auth);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessNewPlayer(const EngineEvent &ent)
{
	std::map<std::string, int>::const_iterator playerID = 
		ent.miData.find("PLAYER_ID");

	assert(playerID != ent.miData.end());

	int id = playerID->second;
	mDataBase->ChangePlayerState(id, true);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessDelPlayer(const EngineEvent &ent) 
{
	std::map<std::string, int>::const_iterator playerID = 
		ent.miData.find("PLAYER_ID");

	assert(playerID != ent.miData.end());

	int id = playerID->second;
	mDataBase->ChangePlayerState(id, false);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessWelcome(const EngineEvent& ent)
{
	std::map<std::string, int>::const_iterator acctId = 
		ent.miData.find("ACCT_ID");

	assert(acctId != ent.miData.end());

	int id = acctId->second;
	mDataBase->ChangeAccountState(id, true);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessRemoveMachine(const EngineEvent& ent) 
{
	std::map<std::string, int>::const_iterator acctId = 
		ent.miData.find("ACCT_ID");

	assert(acctId != ent.miData.end());

	int id = acctId->second;
	mDataBase->ChangeAccountState(id, false);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessCreateCharacter(const EngineEvent& ent) 
{
	std::map<std::string, int>::const_iterator acctId = 
		ent.miData.find("ACCT_ID");
	std::map<std::string, std::string>::const_iterator playerNick =
		ent.msData.find("PLAYER_NICK");

	assert(acctId != ent.miData.end() && playerNick != ent.msData.end());

	mDataBase->CreateCharacter(acctId->second, playerNick->second);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessDeleteCharacter(const EngineEvent& ent) 
{
	std::map<std::string, std::string>::const_iterator playerNick = 
		ent.msData.find("PLAYER_NICK");

	assert(playerNick != ent.msData.end());

	mDataBase->DeleteCharacter(mDataBase->GetPersonIDByNickName(
		playerNick->second));
}
//----------------------------------------------------------------------------
Player DataBaseEngine::GetPlayerByID(int ID, bool &internal, int acctID) 
{
	return mDataBase->GetPlayer(ID, internal, acctID);
}
//----------------------------------------------------------------------------
NPC DataBaseEngine::GetNpcByID (int ID) 
{
	return mDataBase->GetNPC(ID);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessPlayerPos(const EngineEvent &ent) 
{
	std::map<std::string, float>::const_iterator posX =
		ent.mfData.find("POS_X");
	std::map<std::string, float>::const_iterator posY =
		ent.mfData.find("POS_Y");
	std::map<std::string, float>::const_iterator posZ = 
		ent.mfData.find("POS_Z");
	std::map<std::string, float>::const_iterator oW = ent.mfData.find("O_W");
	std::map<std::string, float>::const_iterator oX = ent.mfData.find("O_X");
	std::map<std::string, float>::const_iterator oY = ent.mfData.find("O_Y");
	std::map<std::string, float>::const_iterator oZ = ent.mfData.find("O_Z");

	std::map<std::string, int>::const_iterator playerID = 
		ent.miData.find("PLAYER_ID");

	assert(posX != ent.mfData.end() && posY != ent.mfData.end() &&
		posZ != ent.mfData.end());
	assert(oW != ent.mfData.end() && oX != ent.mfData.end() && 
		oY != ent.mfData.end() && oZ != ent.mfData.end());
	assert(playerID != ent.miData.end());
	
	mDataBase->MajPos(posX->second, posY->second, posZ->second, 
		oW->second, oX->second, oY->second, oZ->second, playerID->second);
}
//----------------------------------------------------------------------------
EngineEvent DataBaseEngine::GetPlayerPos(int ID) 
{
	return mDataBase->GetPlayerPos(ID);
}
//----------------------------------------------------------------------------
EngineEvent DataBaseEngine::GetNpcPos (int ID) 
{
	return mDataBase->GetNPCPos(ID);
}
//----------------------------------------------------------------------------
void DataBaseEngine::ProcessPing(const EngineEvent&)
{
	mDataBase->Ping();
}
//----------------------------------------------------------------------------
void DataBaseEngine::SendNpcInfo(int machineID, int mapID) 
{
	std::vector<EngineEvent> l = mDataBase->NPCInfo(mapID);

	for (std::vector<EngineEvent>::iterator it = l.begin(); 
		it != l.end(); ++it) 
	{
		it->miData["ID"] = machineID;
		SendMessageToNet(*it);
	}
}
//----------------------------------------------------------------------------