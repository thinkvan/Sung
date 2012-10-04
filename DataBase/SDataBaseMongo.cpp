//
// SDataBaseMongo.cpp
//

#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <mongo/client/dbclient.h>
#include "SDataBaseMongo.h"
using namespace mongo;
using namespace Sung;

//----------------------------------------------------------------------------
DataBaseMongo::DataBaseMongo()
{
	mConnection = 0;
	mIsDBOk = false;
}
//----------------------------------------------------------------------------
DataBaseMongo::~DataBaseMongo() 
{
}
//----------------------------------------------------------------------------
bool DataBaseMongo::Open()
{
	if (!mIsDBOk)
	{
		try
		{
			mConnection = new mongo::DBClientConnection;
			mConnection->connect("localhost");
			mIsDBOk = true;

			return true;
		}
		catch (mongo::DBException &e) 
		{
			return false;
		}
	}

	return true;
}
//----------------------------------------------------------------------------
bool DataBaseMongo::Close()
{
	return true;
}
//----------------------------------------------------------------------------
bool DataBaseMongo::Ping() 
{
	return !mConnection->isFailed();
}
//----------------------------------------------------------------------------
bool DataBaseMongo::VerifyPass (const std::string &nickName,
								const std::string &password) 
{
	std::auto_ptr<mongo::DBClientCursor> cursor = 
		mConnection->query("Sung.accounts", QUERY( "name" << nickName ));

	mongo::BSONObj p = cursor->next();

	if (password == p.getStringField("password"))
		return true;

	return false;
}
//----------------------------------------------------------------------------
bool DataBaseMongo::IsAccountConnected(const std::string &nickName) 
{
	std::auto_ptr<mongo::DBClientCursor> cursor =
		mConnection->query("Sung.accounts", QUERY("name" << nickName));

	mongo::BSONObj p = cursor->next();

	if (p.getBoolField("online"))
		return true;

	return false;
}
//----------------------------------------------------------------------------
void DataBaseMongo::ChangeAccountState(int ID, bool online) 
{
	mConnection->update("Sung.accounts", BSON("id" << ID), 
		BSON("online" << online));
}
//----------------------------------------------------------------------------
void DataBaseMongo::ChangePlayerState (int ID, bool online)
{
	mConnection->update("Sung.character", BSON("id" << ID),
		BSON("online" << online));
}
//----------------------------------------------------------------------------
int DataBaseMongo::GetAccountIDByName(const std::string &name) 
{
	std::auto_ptr<mongo::DBClientCursor> cursor =
		mConnection->query("Sung.accounts", QUERY("name" << name));

	mongo::BSONObj p = cursor->next();

	return p.getIntField("id");
}
//----------------------------------------------------------------------------
int DataBaseMongo::GetPersonIDByNickName(const std::string &nickName)
{
	std::auto_ptr<mongo::DBClientCursor> cursor = 
		mConnection->query("Sung.character", QUERY("name" << nickName));

	mongo::BSONObj p = cursor->next();

	return p.getIntField("id");
}
//----------------------------------------------------------------------------
EngineEvent DataBaseMongo::GetCharactersList(int acctID)
{
	EngineEvent e;
	e.mType = EngineEvent::CHARACTERS_LIST;
	e.miData["REQUEST"] = 0;

	std::auto_ptr<mongo::DBClientCursor> cursor = 
		mConnection->query("Sung.character", QUERY("acctid" << acctID));
	for (int i=0; cursor->more(); i++)
	{
		mongo::BSONObj p = cursor->next();

		std::string iAct = stringify<int>(i);

		e.miData["ID" + iAct] = p.getIntField("id");
	}

	return e;
}
//----------------------------------------------------------------------------
void DataBaseMongo::CreateCharacter (int acctID, const std::string &nickName) 
{
	mongo::BSONObj b;
	mongo::BSONObjBuilder p;

	p.genOID().append("acctid", acctID).append("name", nickName);

	b = p.obj();

	//BSONObj b = BSON(GENOID << "acctid" << pAcctId << "name" << pNick);

	mConnection->insert("Sung.character", b);
}
//----------------------------------------------------------------------------
void DataBaseMongo::DeleteCharacter (int ID) 
{
	mConnection->remove("Sung.character", QUERY("id" << ID));
}
//----------------------------------------------------------------------------
Player DataBaseMongo::GetPlayer (int ID, bool &intern, int acctId) 
{	
	std::auto_ptr<mongo::DBClientCursor> cursor = mConnection->query("Sung.character", QUERY("id" << ID));
	mongo::BSONObj b = cursor->next();

	if (b.getIntField("acctid") == acctId)
		intern = true;
	else
		intern = false;	

	Player p(b.getIntField("id"));

	p.SetNickName(b.getStringField("name"));
	p.SetLevel(b.getIntField("level"));	
	p.SetMapID(b.getIntField("map_id"));
	p.SetPosX(b.getField("pos_x").Double());
	p.SetPosY(b.getField("pos_y").Double());
	p.SetPosZ(b.getField("pos_z").Double());
	p.SetOW(b.getField("o_w").Double());
	p.SetOX(b.getField("o_x").Double());
	p.SetOY(b.getField("o_y").Double());
	p.SetOZ(b.getField("o_z").Double());
	p.SetModel(b.getStringField("mesh"));
	p.SetPV(b.getIntField("pv"));
	p.SetPVMax(b.getIntField("pv_max"));

	return p;
}
//----------------------------------------------------------------------------
NPC DataBaseMongo::GetNPC (int ID)
{
	std::auto_ptr<mongo::DBClientCursor> cursor = 
		mConnection->query("Sung.npc", QUERY("id" << ID));
	mongo::BSONObj b = cursor->next();

	NPC n(b.getIntField("id"));
	n.SetNickName(b.getStringField("name"));
	n.SetLevel(b.getIntField("level"));
	n.SetMapID(b.getIntField("map_id"));
	n.SetPosX(b.getField("pos_x").Double());		
	n.SetPosY(b.getField("pos_y").Double());
	n.SetPosZ(b.getField("pos_z").Double());
	n.SetOW(b.getField("o_w").Double());
	n.SetOX(b.getField("o_y").Double());
	n.SetOY(b.getField("o_y").Double());
	n.SetOZ(b.getField("o_z").Double());
	n.SetModel(b.getStringField("mesh"));
	n.SetPV(b.getIntField("pv"));
	n.SetPVMax(b.getIntField("pv_max"));

	return n;
}
//----------------------------------------------------------------------------
void DataBaseMongo::MajPos(float pPosX, float pPosY, float pPosZ, 
						   float pOW, float pOX, float pOY, float pOZ, int pId)
{
	mConnection->update("Sung.characters", BSON("id" << pId),
		BSON("pos_x" << pPosX << "pos_y" << pPosY << "pos_z" << pPosZ
		<< "o_w" << pOW << "o_x" << pOX << "o_y" << pOY << "o_z" << pOZ) );
}
//----------------------------------------------------------------------------
EngineEvent DataBaseMongo::GetPlayerPos(int ID) 
{
	EngineEvent e;

	e.mType = EngineEvent::NPC_POS;
	e.miData["NPC_ID"] = 0;

	std::auto_ptr<mongo::DBClientCursor> cursor =
		mConnection->query("Sung.character", QUERY("id" << ID));

	while (cursor->more()) 
	{
		mongo::BSONObj b = cursor->next();
		e.miData["NPC_ID"] = ID;
		e.mfData["POS_X"] = b.getField("pos_x").Double();
		e.mfData["POS_Y"] = b.getField("pos_y").Double();
		e.mfData["POS_Z"] = b.getField("pos_z").Double();
		e.mfData["O_W"] = b.getField("o_w").Double();
		e.mfData["O_X"] = b.getField("o_x").Double();
		e.mfData["O_Y"] = b.getField("o_y").Double();
		e.mfData["O_Z"] = b.getField("o_z").Double();
	}

	return e;    
}
//----------------------------------------------------------------------------
EngineEvent DataBaseMongo::GetNPCPos (int ID)
{
	EngineEvent e;
	e.mType = EngineEvent::NPC_POS;
	e.miData["NPC_ID"] = 0;

	std::auto_ptr<mongo::DBClientCursor> cursor =
		mConnection->query("Sung.npc", QUERY("id" << ID));

	while (cursor->more()) 
	{
		mongo::BSONObj b = cursor->next();
		e.miData["NPC_ID"] = ID;
		e.mfData["POS_X"] = b.getField("pos_x").Double();
		e.mfData["POS_Y"] = b.getField("pos_y").Double();
		e.mfData["POS_Z"] = b.getField("pos_z").Double();
		e.mfData["O_W"] = b.getField("o_w").Double();
		e.mfData["O_X"] = b.getField("o_x").Double();
		e.mfData["O_Y"] = b.getField("o_y").Double();
		e.mfData["O_Z"] = b.getField("o_z").Double();
	}

	return e; 
}
//----------------------------------------------------------------------------
std::vector<EngineEvent> DataBaseMongo::NPCInfo(int mapID) 
{
	std::vector<EngineEvent> l;
	std::ostringstream s;
	std::auto_ptr<DBClientCursor> cursor =
		mConnection->query("Sung.npc", QUERY("map_id" << mapID));

	EngineEvent e;
	e.mType = EngineEvent::NPC_PROP;
	e.miData["MAP_ID"] = mapID;

	for (int i=0; cursor->more(); i++)
	{
		mongo::BSONObj b = cursor->next();
		NPC n = GetNPC(b.getIntField("id"));
		std::ostringstream archiveStream;
		boost::archive::text_oarchive archive(archiveStream);
		archive << n;
		const std::string& s = archiveStream.str();
		e.msData["NPC_PROP"] = s;

		l.push_back(e);
	}

	return l;
}
//----------------------------------------------------------------------------