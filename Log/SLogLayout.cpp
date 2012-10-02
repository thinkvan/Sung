//
// LogLayout.cpp
//

#include <log4cpp/Priority.hh>
#include <log4cpp/FactoryParams.hh>
#include<boost/date_time/posix_time/posix_time.hpp>
#ifdef LOG4CPP_HAVE_SSTREAM
#include <sstream>
#endif
#include "SLogLayout.h"
using namespace log4cpp;

//----------------------------------------------------------------------------
LogLayout::LogLayout (bool isConsole)
{
	mIsConsole = isConsole;
}
//----------------------------------------------------------------------------
LogLayout::~LogLayout() 
{
}
//----------------------------------------------------------------------------
std::string LogLayout::format(const LoggingEvent& event) 
{
	std::ostringstream message;

	const std::string &priorityName = Priority::getPriorityName(event.priority);

	// adjust name
	std::string catName = event.categoryName;
	while (catName.length() < 12)
		catName += " ";
	if (catName.length() > 12)
		catName.resize(12);

	if (mIsConsole)
	{
		if (priorityName == "DEBUG")
		{
			message << "\033[44m\033[37m(\033[33m" << catName << "\033[37m) " 
			<< priorityName << " " << event.ndc << ": " 
			<< event.message << "\033[0m" << std::endl;
		}
		else if(priorityName == "ERROR")
		{
			message << "\033[41m\033[37m(\033[33m" << catName << "\033[37m) " 
			<< priorityName << " " << event.ndc << ": " 
			<< event.message << "\033[0m" << std::endl;
		}
		else
		{
			message << "(\033[34m" << catName << "\033[0m) " 
			<< priorityName << " " << event.ndc << ": " 
			<< event.message << std::endl;
		}

	}
	else
	{
		message << boost::posix_time::second_clock::local_time() << " (" << event.categoryName << ") " 
		<< priorityName << " " << event.ndc << ": " 
		<< event.message << std::endl;
	}

	return message.str();
}
//----------------------------------------------------------------------------