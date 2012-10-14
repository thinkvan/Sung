//
// SNetMachine.cpp
//

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/bind.hpp>
#include "SNetMachine.h"
#include "STcpSocket.h"
using namespace Sung;

//----------------------------------------------------------------------------
NetMachine::NetMachine()
{
	mTcpSocket = 0;
	mAddress = "";
	mMoveCpt = 0;
	mDeleting = false;	
	mLog = 0;
}
//----------------------------------------------------------------------------
NetMachine::NetMachine(NetEngine *engine, TcpSocket *sock, int ID, int acctId,
					   std::string acctName, Log *log)
					   :
mTcpSocket(sock),
mParent(engine),
mID(ID),
mAcctID(acctId),
mAcctName(acctName),
mLog(log)
{	
	mLog->Debug("Network Log correctly linked");
	
	mPlayer = 0;
	mMoveCpt = 0;
	mDeleting = false;
	mTcpSocket->SetParent(this);
	mAddress = mTcpSocket->GetSocket()->remote_endpoint().address().to_string();
	mTcpSocket->SetBind();
}
//----------------------------------------------------------------------------
NetMachine::NetMachine (NetEngine* engine, int ID, std::string acctName,
						Log *log)
						:
mLog(log),
mParent(engine),
mID(ID),
mMoveCpt(0),
mAcctName(acctName)
{	
	mLog->Debug("Network Log correctly linked");	
}
//----------------------------------------------------------------------------
bool NetMachine::HasAcctName(const std::string &name)
{
	if (mDeleting)
		return false;

	return (name == mAcctName);
}
//----------------------------------------------------------------------------
bool NetMachine::HasNick(const std::string &name)
{
	return (name == GetNick());
}
//----------------------------------------------------------------------------
void NetMachine::TCPSend(const EngineEvent &ent)
{
	if (!mTcpSocket->SendEvent(ent))
	{
		if (mPlayer)
		{
			mLog->Debug(GetAcctName()
							 + " ("
							 + getNick()
							 + ") disconnected on "
							 + m_address);
		}
		else
		{
			mLog->Debug(GetAcctName()
							 + " disconnected on "
							 + mAddress);
		}

		mParent->RemoveMachine(this);
	}
}
//----------------------------------------------------------------------------
void NetMachine::Disconnect(const EngineEvent &ent)
{
	if (mPlayer)
		SaveMove();

	TCPSend(ent);

	mParent->RemoveMachine(this);
}
//----------------------------------------------------------------------------
std::string NetMachine::GetNickName()
{
	if (mPlayer)
		return mPlayer->GetNickName();
	
	return "";
}
//----------------------------------------------------------------------------
int NetMachine::GetPlayerID() 
{
	if (mPlayer)
		return mPlayer->GetID();

	return 1;
}
//----------------------------------------------------------------------------
void NetMachine::SetPlayer (Player player) 
{
	mPlayer = new Player(player);
}
//----------------------------------------------------------------------------
void NetMachine::Move (EngineEvent ent) 
{
	if (ent.mType != EngineEvent::PLAYER_POS
		&& ent.mType != EngineEvent::PLAYER_PROP)
		return;

	mPlayer->Move(ent);
	mMoveCpt++;

	if (mMoveCpt >= 50)
		SaveMove();
}
//----------------------------------------------------------------------------
void NetMachine::Move(float pPosX, float pPosY, float pPosZ, float pOW, float pOX, float pOY, float pOZ)
{
	mPlayer->move(pPosX, pPosY, pPosZ, pOW, pOX, pOY, pOZ);

	mMoveCpt++;

	if (mMoveCpt >= 50)			//TODO : Much less frequent, test value
		SaveMove();
}
//----------------------------------------------------------------------------
void NetMachine::SaveMove()
{
	if (mPlayer) 
	{
		EngineEvent ent;

		ent = mPlayer->GetPos();
		ent.miData["PLAYER_ID"] = GetPlayerID();

		mParent->SendMessageToDataBase(ent);
	}

	mMoveCpt = 0;
}
//----------------------------------------------------------------------------
bool NetMachine::IsRunning() 
{
	return mParent->IsRunning();
}
//----------------------------------------------------------------------------
void NetMachine::PushReceivedEvent(const EngineEvent &ent) 
{
	EngineEvent e = ent;

	ent.miData["ID"] = mID;
	ent.miData["ACCT_ID"] = mAcctID;

	if (mPlayer)
		e.miData["PLAYER_ID"] = GetPlayerID();
	if (e.mType == EngineEvent::CHAT_EVENT)
		e.msData["PLAYER_NICK"] = GetNickName();

	mParent->PushReceivedEvent(e);
}
//----------------------------------------------------------------------------
void NetMachine::SetDeleting() 
{
	mDeleting = true;
}
//----------------------------------------------------------------------------