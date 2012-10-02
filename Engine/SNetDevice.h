//
// SNetDevice.h
//

#ifndef SNETDEVICE_H
#define SNETDEVICE_H

#include <boost/asio.hpp>
#include <string>

namespace Sung
{

	class NetDevice
	{
	public:
		virtual bool IsRunning () = 0;
		virtual void PushReceivedEvent (const EngineEvent &) = 0;
		virtual boost::asio::io_service *GetIO () = 0;
		inline const std::string &GetAddress () const { return mAddress; }

	protected:
		std::string mAddress;
		Log *mLog;
	};

}

#endif