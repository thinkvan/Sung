//
// SScheduledTask.cpp
//

#include <boost/bind.hpp>
#include "SScheduledTask.h"
#include "SScheduleEngine.h"

//----------------------------------------------------------------------------
ScheduledTask::ScheduledTask(float refreshTime, bool runAtExit, 
							 ScheduleEngine *parent)
							 :
mParent(parent),
mIsRunning(false)
{
	mRefreshTime = refreshTime;
	mRunAtExit = runAtExit;
	mIO = parent->getIo();
	mTimer = new boost::asio::deadline_timer((*mIO), 
		boost::posix_time::seconds(mRefreshTime));
}
//----------------------------------------------------------------------------
ScheduledTask::~ScheduledTask() 
{
	Exit();

	delete mTimer;
}
//----------------------------------------------------------------------------
void ScheduledTask::Start() 
{
	mIsRunning = true;

	mTimer->expires_at(mTimer->expires_at() +
		boost::posix_time::seconds(mRefreshTime));
	mTimer->async_wait(boost::bind(&ScheduledTask::Tick, this));
}
//----------------------------------------------------------------------------
void ScheduledTask::Exit()
{
	if (mIsRunning) 
	{
		mIsRunning = false;

		mTimer->cancel();

		if (mRunAtExit)
		{
			Process();
		}
	}
}
//----------------------------------------------------------------------------
void ScheduledTask::Tick() 
{
	if (mIsRunning)
	{
		Process();
		RefreshTimer();
	}
}
//----------------------------------------------------------------------------
void ScheduledTask::RefreshTimer()
{
	mTimer->expires_at(mTimer->expires_at() +
		boost::posix_time::seconds(mRefreshTime));
	mTimer->async_wait(boost::bind(&ScheduledTask::Tick, this));
}
//----------------------------------------------------------------------------