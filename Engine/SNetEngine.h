//
// SNetWorkEngine.h
//

#ifndef SNETENGINE_H
#define SNETENGINE_H

#include <boost/asio.hpp>
#include <sstream>
#include "SNetDevice.h"

#define BUFFER_SIZE 1024

namespace Sung
{
	class NetConsole;
	class NetMachine;
	class TcpSocket;

	class NetworkAuth 
	{
	public:
		std::string	mAcctName;
		int mAcctId;
		TcpSocket *mSocket;
	};

	class NetEngine : public Engine, public NetDevice
	{
	public:
		NetEngine (Game *game, int logPriority, int logOutput);
		~NetEngine();

		void Frame();
		void ProcessReceivedQueue ();
		void ProcessReceivedEvent (const EngineEvent &ent);

		TcpSocket *mTcpSocketCons;
		TcpSocket *mTcpSocket;

		boost::asio::ip::tcp::acceptor *mTcpAcceptorCons;
		boost::asio::ip::tcp::acceptor *mTcpAcceptor;

		boost::asio::io_service *GetIO() { return mIO; }
		inline bool IsRunning() { return mIsRunning; }

		void SendEventMulticast (const EngineEvent &ent);

		int mPortTcpServerConnection;
		int mPortTcpServerCons;

		virtual void PushReceivedEvent(const EngineEvent&);
		int GetNextMachineID();
		int GetActualMachineId();
		int GetNextConsoleID();
		int GetActualConsoleId();

		NetMachine *GetMachineFromID (int);
		NetMachine *GetMachineFromPlayerName (const std::string&);
		NetMachine *GetMachineFromAddress (const std::string&);
		bool IsAddressRegistered (const std::string&);

		std::vector<NetMachine*> mMachines;
		std::vector<NetConsole*> mConsoles;

		void RemoveMachine (NetMachine*);
		void RemoveConsole (NetConsole*);

	private:
		void HandleAcceptTcpCons (const boost::system::error_code&, TcpSocket*);
		void HandleAcceptTcp (const boost::system::error_code&, TcpSocket*);
		void ServerThreadTcpReceiveCons (TcpSocket*);
		void ServerThreadTcpReceive (TcpSocket*);

		void GetConfigData();

		std::queue<EngineEvent> mReceivedEvents;
		boost::mutex mReceivedEventsMutex;

		std::string mMulticastAddress;
		boost::array<char, BUFFER_SIZE> mNetworkBuffer;
		boost::asio::io_service* mIO;
		bool mIsRunning;
		int mNextMachineID;
		int mNextConsoleID;

		std::map<unsigned int, NetworkAuth>	mAuth;
		void AcceptAuth(const NetworkAuth &auth);
		void RejectAuth(const NetworkAuth &auth);
		void DisconnectAcct(const std::string &acctName);

		void ProcessReceivedCommand(const EngineEvent &);

		virtual void ProcessCharactersList(const EngineEvent &e);
		virtual void ProcessRemoveMachine(const EngineEvent &e);
		virtual void ProcessPlayerProp(const EngineEvent &e);
		virtual void ProcessNpcProp(const EngineEvent &e);
		virtual void ProcessEnvironmentInfo(const EngineEvent &e);
		virtual void ProcessAuth(const EngineEvent &e);

		// events received from network
		void ProcessReceivedConsoleCommand(const EngineEvent&);
		void ProcessReceivedCharactersList(const EngineEvent&);
		void ProcessReceivedDelPlayer(const EngineEvent&);
		void ProcessReceivedLoad(const EngineEvent&);
		void ProcessReceivedPlayerPos(const EngineEvent&);
		void ProcessReceivedCreateCharacter(const EngineEvent&);
		void ProcessReceivedDeleteCharacter(const EngineEvent&);
		void ProcessReceivedChatEvent(const EngineEvent&);
		void ProcessReceivedPing(const EngineEvent&);
		void ProcessReceivedPlayerProp(const EngineEvent&);
		void ProcessReceivedNpcProp(const EngineEvent&);

		// unused ones
		void ProcessReceivedMsgNick(const EngineEvent&) {};
		void ProcessReceivedGameProp(const EngineEvent&) {};
		void ProcessReceivedWelcome(const EngineEvent&) {};
		void ProcessReceivedRefused(const EngineEvent&) {};
		void ProcessReceivedNewPlayer(const EngineEvent&) {};
		void ProcessReceivedRunMulti(const EngineEvent&) {};
		void ProcessReceivedSendGameProp(const EngineEvent&) {};
		void ProcessReceivedAddNode(const EngineEvent&) {};
		void ProcessReceivedRemNode(const EngineEvent&) {};
		void ProcessReceivedMoveNode(const EngineEvent&) {};
		void ProcessReceivedRemoveMachine(const EngineEvent&) {};
		void ProcessReceivedRemoveConsole(const EngineEvent&) {};
		void ProcessReceivedItemProp(const EngineEvent&) {};
		void ProcessReceivedTargetChanged(const EngineEvent&) {};
		void ProcessReceivedNpcPos(const EngineEvent&) {};
		void ProcessReceivedEnvironmentInfo(const EngineEvent&) {};
		void ProcessReceivedPlaySound(const EngineEvent&) {};
		void ProcessReceivedAuth(const EngineEvent&) {};
	};

}

#endif