//
// STcpSocket.h
//

#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <boost/asio.hpp>
#include "SLog.h"

#define TCP_BUFFER_SIZE 1024

namespace Sung
{

	class EngineEvent;
	class NetDevice;

	class TcpSocket 
	{
	public:
		TcpSocket (NetDevice *parent, Log *log);
		~TcpSocket ();
		void SetParent (NetDevice*);
		bool SendEvent (const EngineEvent &e);
		void Close ();
		bool SetBind ();
		bool Connect (boost::asio::ip::tcp::endpoint ep);
		EngineEvent SyncReceive();
		inline boost::asio::ip::tcp::socket *GetSocket() { return mSocket; }

	private:
		void AsyncRead(const boost::system::error_code &error,
			std::size_t bytesReceived);

		NetDevice *mParent;
		boost::asio::ip::tcp::socket *mSocket;
		boost::array<char, TCP_BUFFER_SIZE> mBuffer;
		Log *mNetLog;
	};

}

#endif
