//
// STcpSocket.cpp
//

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/bind.hpp>
#include "STcpSocket.h"
#include "SNetDevice.h"
#include "SEngineEvent.h"
using namespace Sung;

#define DEBUG_SEND_SOCKET_INFO
//----------------------------------------------------------------------------
TcpSocket::TcpSocket(NetDevice *parent, Log *log) 
:
mParent(parent) 
{
	mSocket = new boost::asio::ip::tcp::socket((*mParent->GetIO()));	
	mNetLog = log;
}
//----------------------------------------------------------------------------
TcpSocket::~TcpSocket() 
{
	Close();

	delete mSocket;
	mSocket = 0;
}
//----------------------------------------------------------------------------
void TcpSocket::SetParent (NetDevice *parent)
{
	mParent = parent;
}
//----------------------------------------------------------------------------
void TcpSocket::Close() 
{
	if (mSocket)
		mSocket->close();
}
//----------------------------------------------------------------------------
bool TcpSocket::SendEvent (const EngineEvent &event) 
{
	mNetLog->Debug("TcpSocket::SendEvent(event);");

	event.DisplayOnLog(mNetLog);
	mNetLog->Debug("-------------------------");
	
	// 如果网络没有运行，不能发送任何数据
	if (!mParent->IsRunning() || !mSocket->is_open())
		return false;
	
	std::ostringstream archiveStream;
	boost::archive::text_oarchive archive(archiveStream);
	archive << event;
	const std::string &outboundData = archiveStream.str();

	boost::system::error_code error;
	mSocket->write_some(boost::asio::buffer(outboundData), error);
	if (error) 
	{
		mNetLog->Error("Error sending TCP event to client (" + outboundData + ")");
		return false;
	}
	
	mNetLog->Debug("Send (TCP) to " + mParent->GetAddress() + " : " + outboundData);

	return true;
}
//----------------------------------------------------------------------------
void TcpSocket::AsyncRead(const boost::system::error_code& error, 
						  size_t bytesReceived)
{
	mNetLog->Debug("TcpSocket::asyncRead("
					  + stringify<int>(error.value())
					  + "|"
					  + error.message()
					  + ", "
					  + stringify<size_t>(bytesReceived)
					  + ");");

	if (error.value() != 0)
		return;
	
	// let's deserialize the message
	if (bytesReceived > 0) 
	{
		std::string strData(&mBuffer[0], bytesReceived);
		std::istringstream archiveStream(strData);

		EngineEvent ne;

		try
		{
			boost::archive::text_iarchive archive(archiveStream);
			archive >> ne;
		} 
		catch (std::exception &e)
		{
			mNetLog->Error("Exception in TcpSocket::asyncReceive("
							  + std::string(e.what())
							  + ", "
							  + stringify<size_t>(bytesReceived)
							  + " bytes received from client");
			if (mSocket)
			{
				if (mSocket->is_open())
				{
					SetBind();
				}
			}
			return;
		}
		
		mNetLog->Debug("Received from client : " + strData);

		// 增加事件到接受队列
		mParent->PushReceivedEvent(ne);
	}
	
	// an empty packet is received while closing the socket, we must'nt re-open it if it's closing
	if (mSocket)
	{
		if (mSocket->is_open()) {
			SetBind();
		}
	}
}
//----------------------------------------------------------------------------
bool TcpSocket::SetBind() {
	try
	{
		mSocket->async_receive(boost::asio::buffer(mBuffer), 
			boost::bind(&TcpSocket::AsyncRead, this,
			boost::asio::placeholders::error, 
			boost::asio::placeholders::bytes_transferred));
	} 
	catch(std::exception &e) 
	{
		mNetLog->Error("Handled exception handled in TcpSocket::setBind() : "
						  + std::string(e.what()));
		return false;
	} 
	catch(...) 
	{
		mNetLog->Error("Handled unknown exception handled in TcpSocket::setBind()");
		return false;
	}
	return true;
}
//----------------------------------------------------------------------------
bool TcpSocket::Connect(boost::asio::ip::tcp::endpoint ep) 
{
	boost::system::error_code er;

	mSocket->connect(ep, er);
	boost::asio::socket_base::keep_alive option(true);
	mSocket->set_option(option);
	
	// an error occured, we return 1
	if (er.value() != 0) 
	{
		mNetLog->Error("Unable to join host : "
						  + er.message());
		delete mSocket;
		mSocket = NULL;

		return false;
	}

	return true;
}
//----------------------------------------------------------------------------
EngineEvent TcpSocket::SyncReceive() 
{
	EngineEvent ent;
	try
	{
		mSocket->receive(boost::asio::buffer(mBuffer));

		std::string strData(&mBuffer[0], mBuffer.size());
		std::istringstream archiveStream(strData);
		boost::archive::text_iarchive archive(archiveStream);
		archive >> ent;
	}
	catch (std::exception &e) 
	{
		mNetLog->Error("Handled exception in TcpSocket::syncReceive() : "
						 + std::string(e.what()));
	}
	catch (...) 
	{
		mNetLog->Error("Handled unknown exception in TcpSocket::SyncReceive()");
	}

	SetBind();

	return ent;
}
//----------------------------------------------------------------------------