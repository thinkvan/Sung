//
// SScheduleEngine.cpp
//

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "SScheduleEngine.h"
#include "SScheduledTask.h"
#include "SLog.h"
using namespace Sung;
//----------------------------------------------------------------------------
ScheduleEngine::ScheduleEngine (Game *game, int logPriority, int logOutput) 
: 
Engine(game, logPriority, logOutput) 
{
	mIO = new boost::asio::io_service;

	mLog = new Log("Schedule", logPriority, logOutput);

	// add all tasks
	//MysqlPing* mysqlPing = new MysqlPing(MYSQL_PING_DELAY, MYSQL_PING_RUN, this);
	//mTaskList.push_back(mysqlPing);

	for (std::vector<ScheduledTask*>::iterator i = mTaskList.begin(); 
		i != mTaskList.end(); i++)
		(*i)->start();

	boost::thread(boost::bind(&boost::asio::io_service::run, mIO));
}
//----------------------------------------------------------------------------
ScheduleEngine::~ScheduleEngine()
{
	bool ok = true;

	try
	{
		mLog->Info("Running last scheduled tasks...");

		for (std::vector<ScheduledTask*>::iterator i = mTaskList.begin(); 
			i != mTaskList.end(); i++) 
		{
			(*i)->Exit();
			delete *i;
		}

		mTaskList.clear();
		mIO->stop();
		delete mIO;
	} 
	catch(std::exception &e)
	{
		mLog->Error("Handled exception while running last scheduled tasks : " 
			+ std::string(e.what()));
		ok = false;
	} catch(...) 
	{
		mLog->Error("Handled unknown exception while running last scheduled tasks");
		ok = false;
	}
	if (ok)
	{
		mLog->Info("Last scheduled tasks successfully run");
	}
}
//----------------------------------------------------------------------------