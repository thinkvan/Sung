//
// SScheduleEngine.h
//

#ifndef SSCHEDULEENGINE_H
#define SSCHEDULEENGINE_H

#include "SGame.h"
#include "SEngine.h"
#include "SPlayer.h"
#include "SLog.h"

namespace Sung
{

	class ScheduledTask;

	class ScheduleEngine : public Engine 
	{
	public:
		ScheduleEngine(Game*, int logPriority, int logOutput);
		~ScheduleEngine();
		void Frame() { ProcessQueue(); }
		boost::asio::io_service* getIO() { return mIO; };

	private:
		std::vector<ScheduledTask*> mTaskList;
		boost::asio::io_service* mIO;
	};

}

#endif