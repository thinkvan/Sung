//
// SGame.h
//

#ifndef SGAME_H
#define SGAME_H

#ifdef WIN32
#include <WinDef.h>
#endif
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/mutex.hpp>
#include "SLog.h"

namespace Sung
{

	class Engine;
	class NetEngine;
	class DataBaseEngine;
	class ScheduleEngine;

	class Game 
	{
	public:
		static Game* GetInstance(int nbThreads = 1, int log_priority = LOG_TO_FILE | LOG_TO_CONSOLE, int log_output = Log::INFO);
		static void Destroy();
		void Run();
		void RunEngine(Engine* e);
		void RunEngines(Engine* e, Engine* e2);
		void Stop();
		std::string Uptime();
		bool IsStillRunning();

	private:
		Game (int nbThreads, int log_priority, int log_output);
		~Game();
		static Game* mInstance;

		//	GameEngine *m_gameEngine;
		NetEngine *mNetEngine;
		DataBaseEngine *mDataBaseEngine;
		ScheduleEngine *mScheduleEngine;
		boost::mutex mStillRunningMutex;
		bool mIsStillRunning;
		int mNumThreads;
		boost::posix_time::ptime mStartDate;
		Log *mLog;
	};

#ifdef UNIX
	void SigintStop(int signum);
#endif

	std::vector<std::string> explode(std::string pStr, char pSrch);

}

#endif