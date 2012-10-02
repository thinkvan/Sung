//
// SLog.h
//

#ifndef SLOG_H
#define SLOG_H

#include	<log4cpp/Category.hh>
#include	<log4cpp/OstreamAppender.hh>
#include	<log4cpp/FileAppender.hh>
#ifdef UNIX
#include	<log4cpp/SyslogAppender.hh>
#endif
#include	"SLogLayout.h"

#define LOG_TO_FILE					0x8
#define LOG_TO_CONSOLE				0x4
#define LOG_TO_ERROR_CONSOLE		0x2
#ifdef UNIX
#define LOG_TO_SYSLOG				0x1
#endif

#define NO_LOG_TO_FILE				0x7
#define NO_LOG_TO_CONSOLE			0x11
#define NO_LOG_TO_ERROR_CONSOLE		0x13
#ifdef UNIX
#define NO_LOG_TO_SYSLOG			0x14
#endif

#define LOG_ALL						0x15
#define LOG_NOTHING					0x0

namespace Sung
{

	class Log
	{
	public:
		enum Priority
		{
			DEBUG	= log4cpp::Priority::DEBUG,
			INFO	= log4cpp::Priority::INFO,
			NOTICE	= log4cpp::Priority::NOTICE,
			WARN	= log4cpp::Priority::WARN,
			ERROR	= log4cpp::Priority::ERROR,
			CRIT	= log4cpp::Priority::CRIT,
			ALERT	= log4cpp::Priority::ALERT,
			FATAL	= log4cpp::Priority::FATAL,
		};

		Log (const std::string& name, int priority = INFO,
			int output = LOG_TO_FILE|LOG_TO_CONSOLE, 
			const std::string& file = "");

		~Log();

		void Debug (const std::string &message);
		void Info (const std::string &message);
		void Notice (const std::string &message);
		void Warn (const std::string &message);
		void Error (const std::string &message);
		void Crit (const std::string &message);
		void Alert (const std::string &message);
		void Fatal (const std::string &message);

		inline int GetPriority() const { return mPriority; }
		inline int GetOutput() const { return mOutput; }

	private:
		log4cpp::Priority::PriorityLevel Priority(int priority);

		log4cpp::Category *mCategory;		
		int mPriority;
		int mOutput;
	};

}

#endif

