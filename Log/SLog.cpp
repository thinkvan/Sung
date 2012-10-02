//
// SLog.cpp
//

#include <string>
#include	<log4cpp/Category.hh>
#include	<log4cpp/OstreamAppender.hh>
#include	<log4cpp/FileAppender.hh>
#ifdef UNIX
#include	<log4cpp/SyslogAppender.hh>
#endif
#include "SLogLayout.h"
#include "SLog.h"
using namespace Sung;

//----------------------------------------------------------------------------
Log::Log (const std::string &name, int priority, int output, 
		 const std::string &file)
{
	mPriority = priority;
	mOutput = output;

	mCategory = &log4cpp::Category::getInstance(name);
	mCategory->setPriority(priority);

	if ((mOutput & LOG_TO_FILE) != 0)
	{
		std::string filename;
		filename = "./log/";
		(file == "") ? filename+=name : filename+=file;
		filename += ".log";

		log4cpp::LogLayout *layout = new log4cpp::LogLayout();

		log4cpp::FileAppender *appender = new log4cpp::FileAppender("FileAppender", filename);
		appender->setLayout(layout);

		mCategory->addAppender(appender);
	}

	if ((mOutput & LOG_TO_CONSOLE) != 0) 
	{
		log4cpp::LogLayout *layout = new log4cpp::LogLayout(true);

		log4cpp::OstreamAppender *appender = new log4cpp::OstreamAppender("OstreamAppender", &std::cout);
		appender->setLayout(layout);

		mCategory->addAppender(appender);
	}

	if ((mOutput & LOG_TO_ERROR_CONSOLE) != 0) 
	{

		log4cpp::LogLayout *layout = new log4cpp::LogLayout(true);

		log4cpp::OstreamAppender *appender = new log4cpp::OstreamAppender("OstreamAppender", &std::cerr);
		appender->setLayout(layout);

		mCategory->addAppender(appender);
	}

#ifdef UNIX	
	if ((mOutput & LOG_TO_SYSLOG) != 0) 
	{
		log4cpp::LogLayout *layout = new log4cpp::LogLayout();

		log4cpp::SyslogAppender *appender = new log4cpp::SyslogAppender("SyslogAppender", "test"); 
		appender->setLayout(layout);

		mCategory->addAppender(appender);
	}
#endif

	if ((mOutput & LOG_TO_FILE) != 0)
		Info("Logging to file");
	if ((mOutput & LOG_TO_CONSOLE) != 0)
		Info("Logging to cout");
	if ((mOutput & LOG_TO_ERROR_CONSOLE) != 0)
		Info("Logging to cerr");
#ifdef UNIX
	if ((mOutput & LOG_TO_SYSLOG) != 0)
		Info("Logging to syslog");
#endif
}
//----------------------------------------------------------------------------
Log::~Log () 
{
	log4cpp::Category::shutdown();
}
//----------------------------------------------------------------------------
void Log::Debug (const std::string& message)
{
	mCategory->debug(message);
}
//----------------------------------------------------------------------------
void Log::Info (const std::string& message)
{
	mCategory->info(message);
}
//----------------------------------------------------------------------------
void Log::Notice (const std::string& message)
{
	mCategory->notice(message);
}
//----------------------------------------------------------------------------
void Log::Warn (const std::string& message) 
{
	mCategory->warn(message);
}
//----------------------------------------------------------------------------
void Log::Error (const std::string& message) 
{
	mCategory->error(message);
}
//----------------------------------------------------------------------------
void Log::Crit (const std::string& message)
{
	mCategory->crit(message);
}
//----------------------------------------------------------------------------
void Log::Alert (const std::string& message)
{
	mCategory->alert(message);
}
//----------------------------------------------------------------------------
void Log::Fatal (const std::string& message) 
{
	mCategory->fatal(message);
}

//----------------------------------------------------------------------------