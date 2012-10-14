//
// SScheduledTask.h
//

#ifndef SSCHEDULEDTASK_H
#define SSCHEDULEDTASK_H

#include <boost/asio.hpp>

namespace Sung
{

	class ScheduleEngine;

	class ScheduledTask 
	{
	public:
		ScheduledTask (float refreshTime, bool runAtExit, ScheduleEngine *parent);
		virtual ~ScheduledTask ();
		virtual void Process () {};
		void Start();
		void Exit();
		void Tick();
		ScheduleEngine *GetParent() { return mParent; };

	private:
		void RefreshTimer();

		boost::asio::deadline_timer* mTimer;
		boost::asio::io_service* mIO;
		ScheduleEngine *mParent;
		float mRefreshTime;
		bool mRunAtExit;
		bool mIsRunning;
	};

}

#endif