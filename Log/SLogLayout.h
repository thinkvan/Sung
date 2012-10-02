//
// SLogLayout.h
//

#ifndef SLOGLAYOUT_H
#define SLOGLAYOUT_H

#include <log4cpp/Portability.hh>
#include <log4cpp/Layout.hh>

namespace log4cpp 
{

	class LogLayout : public Layout 
	{
	public:
		LogLayout (bool isConsole = false);
		virtual ~LogLayout();

		virtual std::string format (const LoggingEvent& event);

	private:
		bool mIsConsole;
	};

}

#endif