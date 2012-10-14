//
// SNetConsole.cpp
//

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/bind.hpp>
#include "STcpSocket.h"
#include "SNetConsole.h"
using namespace Sung;

//----------------------------------------------------------------------------
NetConsole::NetConsole() 
{
	mTcpSocket = 0;
	mAddress = "";	
	mLog = 0;
}
//----------------------------------------------------------------------------
NetConsole::NetConsole (NetEngine *engine, TcpSocket *sock, int ID, Log *log)
: 
mTcpSocket(sock),
mParent(engine),
mID(ID),
mLog(log)
{
	mLog->Debug("Network Log correctly linked");	
	mTcpSocket->SetParent(this);
	mAddress = mTcpSocket->GetSocket()
		->remote_endpoint().address().to_string();
	
	// 开启socket异步接收
	mTcpSocket->SetBind();
}
//----------------------------------------------------------------------------
NetConsole::~NetConsole ()
{
	if (mTcpSocket)
		mTcpSocket->Close();

	delete mTcpSocket;
}
//----------------------------------------------------------------------------
void NetConsole::TCPSend(const EngineEvent &ent)
{
	if (!mTcpSocket->SendEvent(ent)) 
	{
		mLog->Info("Console disconnected on " + mAddress);

		EngineEvent e;
		e.mType = EngineEvent::REMOVE_CONSOLE;
		e.miData["CONSOLE_ID"] = mID;

		mParent->PushEvent(e);
	}
}
//----------------------------------------------------------------------------
void NetConsole::Disconnect (const EngineEvent &ent)
{
	TCPSend(ent);

	EngineEvent e;
	e.mType = EngineEvent::REMOVE_CONSOLE;
	e.miData["CONSOLE_ID"] = mID;

	mParent->PushEvent(e);
}
//----------------------------------------------------------------------------
bool NetConsole::IsRunning() 
{
	return mParent->IsRunning();
}
//----------------------------------------------------------------------------
void NetConsole::PushReceivedEvent (const EngineEvent &ent)
{
	EngineEvent e = ent;

	e.miData["CONSOLE_ID"] = mID;

	mParent->PushReceivedEvent(e);
}
//----------------------------------------------------------------------------