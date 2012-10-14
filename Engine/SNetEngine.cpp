//
// SNetEngine.cpp
//

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/bind.hpp>
#include "SNetEngine.h"
#include "SNetMachine.h"
#include "SConvert.h"
#include "SConfigFileParser.h"
#include "SLog.h"
using namespace Sung;

//----------------------------------------------------------------------------
NetEngine::NetEngine(Game *game, int logPriority, int logOutput)
: 
Engine(game, logPriority, logOutput) 
{	
	mLog = new Log("Network", logPriority, logOutput);
	
	mTcpAcceptorCons = 0;
	mTcpAcceptor = 0;
	mIsRunning = true;
	mNextConsoleID = 1;
	mNextMachineID = 1;

	mIO = new boost::asio::io_service;

	try 
	{
		GetConfigData();
	}
	catch(std::exception &e) 
	{
		mLog->Error("Handled exception in NetEngine::NetEngine() : " 
			+ std::string(e.what()));
	}
	catch(std::string& e)
	{
		mLog->Error("Handled exception in NetEngine::NetEngine() : " + e);

		mIsRunning = false;
	}
	catch(...) 
	{
		mLog->Error("Handled unknown exception in NetEngine::NetEngine()");
	}
}
//----------------------------------------------------------------------------
NetEngine::~NetEngine() 
{
	bool ok = true;

	mLog->Info("Closing connexions...");

	try
	{
		EngineEvent e;
		e.mType = EngineEvent::REFUSED;

		for (std::vector<NetConsole*>::iterator it = mConsoles.begin();
			it != mConsoles.end(); it++)
		{
			(*it)->Disconnect(e);
			delete (*it);
		}
		for (std::vector<NetMachine*>::iterator it = mMachines.begin();
			it != mMachines.end(); it++) 
		{
			(*it)->Disconnect(e);
			delete (*it);
		}

		if (mTcpSocketCons) 
		{
			mTcpSocketCons->Close();
			delete mTcpSocketCons;
		}
		if (mTcpSocket) 
		{
			mTcpSocket->Close();
			delete mTcpSocket;
		}

		mIO->stop();

		delete mTcpAcceptorCons;
		delete mTcpAcceptor;
		delete mIO;
	} 
	catch (std::exception&e) 
	{
		mLog->Error("Handled exception while closing connexions : "
			+ std::string(e.what()));
		ok = false;
	}
	catch (...) 
	{
		mLog->Error("Handled unknown exception while closing connexions");
		ok = false;
	}

	if (ok)
	{
		mLog->Info("Connexions successfully closed");
	}
}
//----------------------------------------------------------------------------
void NetEngine::ProcessEnvironmentInfo(const EngineEvent &ent)
{
	int i = 0;
	std::map<std::string, int>::const_iterator it = ent.miData.find("ID");

	assert(it != ent.miData.end());

	int id = it->second;

	NetMachine *m = GetMachineFromID(id);

	if (m)
	{
		m->TCPSend(ent);
	}
}
//----------------------------------------------------------------------------
void NetEngine::ProcessNpcProp (const EngineEvent &ent)
{
	int i = 0;
	std::map<std::string, int>::const_iterator it = ent.miData.find("ID");

	assert(it != ent.miData.end());

	int id = it->second;

	NetMachine *m = GetMachineFromID(id);

	if (m != 0)
		m->TCPSend(ent);
}
//----------------------------------------------------------------------------
void NetEngine::ProcessPlayerProp(const EngineEvent &ent)
{
	int i = 0;
	std::map<std::string, int>::const_iterator it = ent.miData.find("ID");

	assert(it != ent.miData.end());

	int id = it->second;

	NetMachine *m = GetMachineFromID(id);
	
	if (m != 0)
		m->TCPSend(ent);
}
//----------------------------------------------------------------------------
void NetEngine::ProcessRemoveMachine(const EngineEvent &ent)
{
	std::map<std::string, int>::const_iterator idIt = ent.miData.find("ID");

	assert(idIt != ent.miData.end());

	int id = idIt->second;

	for (unsigned int i = 0; i < mMachines.size(); i++) 
	{
		if (mMachines.at(i)->GetID() == id)
		{
			mMachines.at(i)->SaveMove();

			EngineEvent e;
			e.mType = EngineEvent::REMOVE_MACHINE;
			e.miData["ACCT_ID"] = mMachines.at(i)->GetAcctId();
			SendMessageToDataBase(e);

			mMachines.erase(mMachines.begin() + i);

			return;
		}
	}
}
//----------------------------------------------------------------------------
void NetEngine::ProcessCharactersList(const EngineEvent &ent)
{
	std::map<std::string, int>::const_iterator it = ent.miData.find("ID");

	assert(it != ent.miData.end());

	int id = it->second;

	NetMachine *m = GetMachineFromID(id);

	if (m != 0)
		m->TCPSend(ent);
}
//----------------------------------------------------------------------------
void NetEngine::ProcessAuth(const EngineEvent &ent)
{
	std::map<std::string, int>::const_iterator accepted =
		ent.miData.find("ACCEPTED");
	std::map<std::string, int>::const_iterator id = ent.miData.find("ID");
	std::map<std::string, int>::const_iterator acctId = 
		ent.miData.find("ACCT_ID");

	assert(accepted != ent.miData.end() && id != ent.miData.end());

	std::map<unsigned int, NetworkAuth>::const_iterator it = mAuth.find(id->second);
	if (it == mAuth.end())
		return;

	NetworkAuth auth = it->second;

	if (accepted->second == 0)
	{
		RejectAuth(auth);
		return;
	}

	assert(acctId != ent.miData.end());

	auth.mAcctId = acctId->second;
	AcceptAuth(auth);
}
//----------------------------------------------------------------------------
void NetEngine::GetConfigData()
{
	// initialize ports
	ConfigFileParser config("./Config/network.conf");
	mPortTcpServerConnection = config.Read("port_tcp_server_connection", 8001);
	mPortTcpServerCons = config.Read("port_tcp_server_console", 8002);

	// 开启网络，打开所有异步操作
	mLog->Info("Network initialisation...");

	//TCP (console)
	mTcpSocketCons = new TcpSocket(this, mLog);
	mTcpAcceptorCons = new boost::asio::ip::tcp::acceptor(*mIO, 
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
		mPortTcpServerCons));
	boost::system::error_code ec;
	mTcpAcceptorCons->async_accept(*mTcpSocketCons->GetSocket(),
		boost::bind(&NetEngine::HandleAcceptTcpCons, this, ec,
		mTcpSocketCons));

	if (!mTcpAcceptorCons->is_open())
	{
		throw std::string("Cannot start network");
	}

	mLog->Info("TCP (console) listening @ "
		+ mTcpAcceptorCons->local_endpoint().address().to_string()
		+ ":"
		+ stringify<short unsigned int>(
			mTcpAcceptorCons->local_endpoint().port()));

	//TCP
	mTcpSocket = new TcpSocket(this, mLog);
	mTcpAcceptor = new boost::asio::ip::tcp::acceptor(*mIO,
		boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),
		mPortTcpServerConnection));
	mTcpAcceptor->async_accept(*mTcpSocket->GetSocket(), 
		boost::bind(&NetEngine::HandleAcceptTcp, this, ec, mTcpSocket));

	if (!mTcpAcceptor->is_open())
	{
		throw std::string("Cannot start network");
	}

	mLog->Info("TCP (console) listening @ "
		+ mTcpAcceptor->local_endpoint().address().to_string()
		+ ":"
		+ stringify<short unsigned int>(
			mTcpAcceptor->local_endpoint().port()));

	// run
	boost::thread(boost::bind(&boost::asio::io_service::run, mIO));
}
//----------------------------------------------------------------------------
void NetEngine::HandleAcceptTcpCons(const boost::system::error_code &error,
									TcpSocket *socket) 
{
	if (error.value() != 0)
	{
		mLog->Error("Something has hapenned with the socket...");
		return;
	}

	boost::asio::socket_base::keep_alive option(true);
	socket->GetSocket()->set_option(option);

	boost::thread t(boost::bind(&NetEngine::ServerThreadTcpReceiveCons,
		this, socket));


	// 重新开始接受一个新的socket
	mLog->Info("Rearming the accept...");
	mTcpSocketCons = new TcpSocket(this, mLog);

	boost::system::error_code err;
	mTcpAcceptorCons->async_accept(*mTcpSocketCons->GetSocket(), 
		boost::bind(&NetEngine::HandleAcceptTcpCons, this, err,
		mTcpSocketCons));
	mLog->Info("Accept rearmed.");
}
//----------------------------------------------------------------------------
void NetEngine::HandleAcceptTcp(const boost::system::error_code &error,
								TcpSocket *socket)
{
	if (error.value() != 0) 
	{
		mLog->Error("Something has hapenned with the socket...");
		return;
	}

	boost::asio::socket_base::keep_alive option(true);
	socket->GetSocket()->set_option(option);

	// start the thread
	boost::thread t(boost::bind(&NetEngine::ServerThreadTCPReceive, this,
		socket));

	// 重新开始接受一个新的socket
	mLog->Info("Rearming the accept...");
	mTcpSocket = new TcpSocket(this, mLog);

	boost::system::error_code err;
	mTcpAcceptor->async_accept(*mTcpSocket->GetSocket(),
		boost::bind(&NetEngine::HandleAcceptTcp, this, err, mTcpSocket));
	mLog->Info("Accept rearmed.");
}
//----------------------------------------------------------------------------
void NetEngine::ServerThreadTcpReceiveCons (TcpSocket *socket) 
{
	NetConsole *n = new NetConsole(this, socket, GetNextConsoleID(), mLog);

	EngineEvent e;
	e.mType = EngineEvent::WELCOME;

	mConsoles.push_back(n);

	// 从现在开始，所有的“沟通”通过NetConsole，不是通过socket
	n->TCPSend(e);
}
//----------------------------------------------------------------------------
void NetEngine::ServerThreadTCPReceive (TcpSocket *socket)
{
	EngineEvent ne = socket->SyncReceive();

	if (ne.mType != EngineEvent::MSG_NICK)
	{
		EngineEvent e;
		e.mType = EngineEvent::MSG_NICK;
		e.miData["ACCEPTED"] = 0;
		e.msData["REASON"] = "You must give your nick before anything";

		socket->SendEvent(e);
		socket->Close();

		return;
	}
	
	mLog->Info("EngineEvent::MSG_NICK received : "
					+ ne.sData["ACCT_NAME"]
					+ " - "
					+ ne.sData["PASS"]);

	// nick is empty, player is refused
	if (ne.msData["ACCT_NAME"] == "")
	{
		EngineEvent e;
		e.mType = EngineEvent::MSG_NICK;
		e.miData["ACCEPTED"] = 0;
		e.msData["REASON"] = "Player's name cannot be empty";
		
		mLog->Info("Nick empty, refused.");
		
		socket->SendEvent(e);
		socket->Close();

		return;
	}
	
	// TODO send password by ssl, so on another socket
	// Password check
	NetworkAuth auth;
	static unsigned int id = 0;
	id++;
	auth.mAcctName = ne.msData["ACCT_NAME"];
	auth.mSocket = socket;
	mAuth.insert(std::pair<int, NetworkAuth>(id, auth));
	EngineEvent eauth;
	eauth.mType = EngineEvent::AUTH;
	eauth.miData["ID"] = id;
	eauth.msData["ACCT_NAME"] = auth.mAcctName;
	eauth.msData["PASS"] = ne.msData["PASS"];
	SendMessageToDataBase(eauth);
/*
	// wrong password
	// /!\ don't call directly another engine function, use engine events
	if (!getDbEngine()->verifyPass(ne.msData["ACCT_NAME"], ne.msData["PASS"])) {
		rejectAuth(XXX);
	}

	// all the tests are successfull, let's add the player
	// /!\ don't call directly another engine function, use engine events
	int acctId = getDbEngine()->getAcctIdByName(ne.msData["ACCT_NAME"]);
	
	// disconnect all network machines using this nick
	// /!\ don't call directly another engine function, use engine events
	if (getDbEngine()->isAccountConnected(ne.msData["ACCT_NAME"])) {
		bool ok = false;
		for (std::vector<NetMachine*>::iterator it = m_lMachines.begin(); !ok && it != m_lMachines.end(); it++) {
			if (!(*it)->deleting() && (*it)->hasAcctName(ne.msData["ACCT_NAME"])) {
				ok = true;
				
				engineLog->info(ne.msData["ACCT_NAME"]
								+ " is connected from another computer. Disconecting.");
				
				EngineEvent deco;
				deco.m_type = EngineEvent::REMOVE_MACHINE;
				deco.msData["REASON"] = "Connected from another computer";
				(*it)->disconnect(deco);
			}
		}

	}

acceptAuth(XXX);*/

}
//----------------------------------------------------------------------------
void NetEngine::AcceptAuth (const NetworkAuth &auth)
{
	// create a new network machine & player
	// send OK
	NetMachine *n = new NetMachine(this, auth.mSocket, GetNextMachineID(), 
		auth.mAcctId, auth.mAcctName, mLog);

	// setting account online on DB
	EngineEvent e;
	e.mType = EngineEvent::WELCOME;
	e.miData["ACCT_ID"] = auth.mAcctID;
	SendMessageToDataBase(e);

	mMachines.push_back(n);

	// from now, all the communication is done through the network machine and not through the socket anymore
	EngineEvent accept;
	accept.mType = EngineEvent::MSG_NICK;
	accept.miData["ACCEPTED"] = 1;
	n->TCPSend(accept);
}
//----------------------------------------------------------------------------
void NetEngine::RejectAuth(const NetworkAuth &auth) 
{
	EngineEvent e;
	e.mType = EngineEvent::MSG_NICK;
	e.miData["ACCEPTED"] = 0;
	e.msData["REASON"] = "Wrong password";

	mLog->Info("Wrong password, refused to " + auth.mAcctName);

	auth.mSocket->SendEvent(e);
	auth.mSocket->Close();

	return;
}
//----------------------------------------------------------------------------
void NetEngine::DisconnectAcct(const std::string& acctName) 
{
	int acctId = -1;
	for (NetMachine* machine = GetMachineFromPlayerName(acctName); 
		machine != 0;
		machine = GetMachineFromPlayerName(acctName))
	{
		mLog->info(acctName +
			" is connected from another computer. Disconecting.");
		acctId = machine->GetAcctId();

		EngineEvent deco;
		deco.mType = EngineEvent::REMOVE_MACHINE;
		deco.msData["REASON"] = "Connected from another computer";
		machine->Disconnect(deco);
	}

	if (acctId > -1)
	{
		EngineEvent deco;
		deco.mType = EngineEvent::REMOVE_MACHINE;
		deco.miData["ACCT_ID"] = acctId;
		SendMessageToDataBase(deco);
	}
}
//----------------------------------------------------------------------------
int NetEngine::GetNextMachineID() 
{
	return mNextMachineID++;
}
//----------------------------------------------------------------------------
int NetEngine::GetNextConsoleID()
{
	return mNextConsoleID++;
}
//----------------------------------------------------------------------------
int NetEngine::GetActualMachineID()
{
	return mNextMachineID;
}
//----------------------------------------------------------------------------
int NetEngine::GetActualConsoleID()
{
	return mNextConsoleID;
}
//----------------------------------------------------------------------------
void NetEngine::PushReceivedEvent(const EngineEvent &ent) 
{
	boost::mutex::scoped_lock lock(mReceivedEventsMutex);

	mReceivedEvents.push(ent);
}
//----------------------------------------------------------------------------
bool NetEngine::IsAddressRegistered (const std::string &addre) 
{
	for (unsigned int i = 0; i <(unsigned int)mMachines.size(); i++)
	{
		if (mMachines.at(i)->GetAddress() == addre)
			return true;
	}

	return false;
}
//----------------------------------------------------------------------------
NetMachine* NetEngine::GetMachineFromID(int ID)
{
	for (unsigned int i = 0; i < mMachines.size(); i++)
	{
		if (mMachines.at(i)->GetID() == ID)
		{
			return mMachines.at(i);
		}
	}

	mLog->Error("Unknown result in NetEngine::GetMachineFromID("
		+ stringify<int>(ID) + "), 0 returned");

	return 0;
}
//----------------------------------------------------------------------------
NetMachine* NetEngine::GetMachineFromPlayerName (const std::string &name)
{
	for (unsigned int i = 0; i < mMachines.size(); i++)
	{
		if (mMachines.at(i)->HasNick(name))
			return mMachines.at(i);
	}

	mLog->Error("Unknown result in NetEngine::GetMachineFromPlayerName("
		+ name + "), 0 returned");

	return 0;
}
//----------------------------------------------------------------------------
NetMachine* NetEngine::GetMachineFromAddress(const std::string &name) 
{
	for (unsigned int i = 0; i < mMachines.size(); i++)
	{
		if (mMachines.at(i)->GetAddress() == name)
			return mMachines.at(i);
	}

	mLog->Error("Unknown result in NetEngine::GetMachineFromAddress("
		+ name + "), NULL returned");

	return 0;
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedQueue()
{
	while (!mReceivedEvents.empty()) 
	{
		boost::mutex::scoped_lock lock(mReceivedEventsMutex);

		EngineEvent e = mReceivedEvents.front();
		mReceivedEvents.pop();

		lock.unlock();

		ProcessReceivedEvent(e);
	}
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedCharactersList (const EngineEvent &ent) 
{
	std::map<std::string, int>::const_iterator it = ent.miData.find("REQUEST");
	assert(it != ent.miData.end());

	if (it->second != 1)
		return;

	SendMessageToDataBase(ent);
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedDelPlayer(const EngineEvent &ent) 
{
	std::map<std::string, std::string>::const_iterator it = 
		ent.msData.find("PLAYER_NICK");
	assert(it != ent.msData.end());

	std::string playerNick = it->second;

	EngineEvent eDelPlayer;
	eDelPlayer.mType = EngineEvent::DEL_PLAYER;

	// send "del player" to all already connected machines
	eDelPlayer.msData["PLAYER_NICK"] = playerNick;
	NetMachine* m = GetMachineFromPlayerName(playerNick);
	if (m != 0) 
	{
		eDelPlayer.miData["PLAYER_ID"] = m->GetPlayerID();

		SendMessageToDataBase(eDelPlayer);

		for (unsigned int i = 0; i < mMachines.size(); i++)
		{
			if (!mMachines.at(i)->HasNick(playerNick))
				mMachines.at(i)->TCPSend(eDelPlayer);
		}
	}
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedLoad(const EngineEvent &ent)
{
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedPlayerPos(const EngineEvent &ent) 
{
	std::map<std::string, int>::const_iterator it = ent.miData.find("ID");
	assert(it != ent.miData.end());

	int id = it->second;
	NetMachine* m = GetMachineFromID(id);
	if (m == 0)
		return;

	if (m->mPlayer)
	{
		m->Move(ent);

		EngineEvent e = ent;

		e.miData["PLAYER_ID"] = m->GetPlayerID();
		for (unsigned int i = 0; i < mMachines.size(); i++)
		{
			if (mMachines.at(i)->GetID() != id)
				mMachines.at(i)->TCPSend(e);
		}
	}
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedCreateCharacter(const EngineEvent &ent)
{
	SendMessageToDataBase(ent);
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedDeleteCharacter(const EngineEvent &ent) 
{
	SendMessageToDataBase(ent);
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedChatEvent (const EngineEvent &ent)
{
	std::map<std::string, std::string>::const_iterator it = 
		ent.msData.find("MESSAGE");
	assert(it != ent.msData.end());

	std::string message = it->second;
	if (message.at(0) == '.') 
	{
		ProcessReceivedCommand(ent);
		return;
	}

	for (unsigned int i = 0; i < mMachines.size(); i++)
	{
		mMachines.at(i)->TCPSend(ent);
	}
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedPing (const EngineEvent &ent) 
{
	std::map<std::string, int>::const_iterator stateIt =
		ent.miData.find("STATE");
	std::map<std::string, int>::const_iterator idIt = ent.miData.find("ID");
	assert(stateIt != ent.miData.end() && idIt != ent.miData.end());

	int state = stateIt->second;
	int id = idIt->second;
	if (state == 0) 
	{
		for (unsigned int i=0; i<(unsigned int)mMachines.size(); i++)
			if (mMachines.at(i)->getId() == id) 
			{
				EngineEvent e = ent;
				e.miData["STATE"] = 1;
				mMachines.at(i)->TCPSend(e);

				return;
			}
	}
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedPlayerProp(const EngineEvent &ent)
{
	SendMessageToDataBase(ent);
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedNpcProp(const EngineEvent &ent) 
{
	SendMessageToDataBase(ent);
}
//----------------------------------------------------------------------------
void NetEngine::ProcessReceivedEvent(const EngineEvent &ent) 
{
	static struct 
	{ 
		EngineEvent::type t;
		void (NetEngine::*f)(const EngineEvent&);
	} tab[] = 
	{
		{EngineEvent::MSG_NICK, &NetEngine::ProcessReceivedMsgNick},
		{EngineEvent::CHARACTERS_LIST, &NetEngine::ProcessReceivedCharactersList},
		{EngineEvent::GAME_PROP, &NetEngine::ProcessReceivedGameProp},
		{EngineEvent::WELCOME, &NetEngine::ProcessReceivedWelcome},
		{EngineEvent::REFUSED, &NetEngine::ProcessReceivedRefused},
		{EngineEvent::NEW_PLAYER, &NetEngine::ProcessReceivedNewPlayer},
		{EngineEvent::RUN_MULTI, &NetEngine::ProcessReceivedRunMulti},
		{EngineEvent::SEND_GAME_PROP, &NetEngine::ProcessReceivedSendGameProp},
		{EngineEvent::DEL_PLAYER, &NetEngine::ProcessReceivedDelPlayer},
		{EngineEvent::LOAD, &NetEngine::ProcessReceivedLoad},
		{EngineEvent::ADD_NODE, &NetEngine::ProcessReceivedAddNode},
		{EngineEvent::REM_NODE, &NetEngine::ProcessReceivedRemNode},
		{EngineEvent::MOVE_NODE, &NetEngine::ProcessReceivedMoveNode},
		{EngineEvent::REMOVE_MACHINE, &NetEngine::ProcessReceivedRemoveMachine},
		{EngineEvent::REMOVE_CONSOLE, &NetEngine::ProcessReceivedRemoveConsole},
		{EngineEvent::PLAYER_POS, &NetEngine::ProcessReceivedPlayerPos},
		{EngineEvent::NPC_POS, &NetEngine::ProcessReceivedNpcPos},
		{EngineEvent::CREATE_CHARACTER, &NetEngine::ProcessReceivedCreateCharacter},
		{EngineEvent::DELETE_CHARACTER, &NetEngine::ProcessReceivedDeleteCharacter},
		{EngineEvent::ITEM_PROP, &NetEngine::ProcessReceivedItemProp},
		{EngineEvent::CHAT_EVENT, &NetEngine::ProcessReceivedChatEvent},
		{EngineEvent::PING, &NetEngine::ProcessReceivedPing},
		{EngineEvent::TARGET_CHANGED, &NetEngine::ProcessReceivedTargetChanged},
		{EngineEvent::PLAYER_PROP, &NetEngine::ProcessReceivedPlayerProp},
		{EngineEvent::NPC_PROP, &NetEngine::ProcessReceivedNpcProp},
		{EngineEvent::CONSOLE_COMMAND, &NetEngine::ProcessReceivedConsoleCommand},
		{EngineEvent::ENVIRONMENT_INFO, &NetEngine::ProcessReceivedEnvironmentInfo},
		{EngineEvent::PLAY_SOUND, &NetEngine::ProcessReceivedPlaySound},
		{EngineEvent::AUTH, &NetEngine::ProcessAuth}
	};

	(this->*tab[ent.mType].f)(ent);
}
//----------------------------------------------------------------------------
void NetEngine::RemoveMachine (NetMachine* machine)
{
	if (machine->Deleting())
		return;

	for (std::vector<NetMachine*>::iterator it = mMachines.begin();
		it != mMachines.end(); it++) 
	{
		if ((*it) == machine)
		{
			machine->SetDeleting();

			EngineEvent e;
			e.mType = EngineEvent::REMOVE_MACHINE;
			e.miData["ACCT_ID"] = machine->GetAcctID();
			SendMessageToDataBase(e);

			mMachines.erase(it);

			return;
		}
	}
}
//----------------------------------------------------------------------------
void NetEngine::RemoveConsole(NetConsole* console) 
{
	for (unsigned int i = 0; i<(unsigned int)mConsoles.size(); i++) 
	{
		if (mConsoles.at(i) == console) 
		{
			mConsoles.erase(mConsoles.begin() + i);

			return;
		}
	}
}
//----------------------------------------------------------------------------
void NetEngine::Frame() 
{
	ProcessQueue();
	ProcessReceivedQueue();
}
//----------------------------------------------------------------------------