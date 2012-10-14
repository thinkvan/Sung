//
// SNetConsole.h
//

#ifndef SNETCONSOLE_H
#define SNETCONSOLE_H

#include <boost/asio.hpp>
#include <list>
#include "SEngineEvent.h"
#include "SPlayer.h"
#include "SNetDevice.h"
#include "SNetEngine.h"
#include "SLog.h"

#define BUFFER_SIZE 1024

namespace Sung
{

	class TcpSocket;

	class NetConsole : public NetDevice 
	{
	public:
		NetConsole ();
		NetConsole (NetEngine *engine, TcpSocket *sock, int ID, Log *log);
		~NetConsole ();

		void TCPSend (const EngineEvent &ent);

		boost::asio::io_service* GetIO () { return mParent->GetIO(); }
		int GetID() { return mID; }

		void Disconnect (const EngineEvent &ent);
		bool IsRunning ();
		virtual void PushReceivedEvent (const EngineEvent &ent);

	private:
		NetEngine *mParent;
		boost::array<char, BUFFER_SIZE> mNetBuffer;
		TcpSocket *mTcpSocket;
		int mID;
	};

}

#endif