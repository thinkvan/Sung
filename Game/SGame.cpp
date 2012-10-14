//
// SGame.cpp
//

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "SGame.h"
#include "SDataBaseMongo.h"

#include "engine.h"
#include "DB/db_engine.h"
#include "CommunicationClients/network_engine.h"
#include "Environnement/environment_engine.h"
#include "TachesPlanifiees/schedule_engine.h"

#include "../Log/log.h"



#ifdef UNIX

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <signal.h>
#endif

Game* Game::mInstance = 0;
//----------------------------------------------------------------------------
Game* Game::GetInstance(int nbThreads, int logPriority, int logOutput) 
{
	if (!mInstance)
		mInstance = new Game(nbThreads, logPriority, logOutput);

	return mInstance;
}
//----------------------------------------------------------------------------
void Game::Destroy() 
{
	delete mInstance;
}
//----------------------------------------------------------------------------
Game::Game(int numThreads, int logPriority, int logOutput) 
:
mNumThreads(numThreads)
{
	mIsStillRunning = true;
	
	mStartDate = boost::posix_time::second_clock::local_time();	
	mLog = new Log("Server", logPriority, logOutput);	
	mLog->Debug("Creating the engines...");	
	
	mNetEngine = new NetEngine(this, logPriority, logOutput);
	mDataBaseEngine = new DataBaseEngine(this, logPriority, logOutput);
	mScheduleEngine = new ScheduleEngine(this, logPriority, logOutput);

	mLog->Debug("Linking the engines together...");

	mNetEngine->AttachDataBaseEngine(mDataBaseEngine);
	mNetEngine->AttachScheduleEngine(mScheduleEngine);

	mDataBaseEngine->AttachNetEngine(mNetEngine);
	mDataBaseEngine->AttachScheduleEngine(mScheduleEngine);

	mScheduleEngine->AttachNetEngine(mNetEngine);
	mScheduleEngine->AttachDataBaseEngine(mDataBaseEngine);
}
//----------------------------------------------------------------------------
Game::~Game() 
{
	try 
	{
		mLog->Debug("Destroying the engines...");

		delete mNetEngine;
		delete mScheduleEngine;
		delete mDataBaseEngine;
	} 
	catch(std::exception &e) 
	{
		mLog->Error("Handled exception in ~Game() : " + std::string(e.what()));
	} catch(...) 
	{
		mLog->Error("Handled unknown exception in ~Game()");
	}
}
//----------------------------------------------------------------------------
void Game::Run()
{
	
#ifdef UNIX
	if (signal(SIGINT, SigintStop) == SIG_ERR)
		mLog->Error("Error while intercepting Sigint");
#endif

	
	if (mNumThreads > 0) 
	{
		boost::thread_group group;

		group.create_thread(boost::bind(&Game::RunEngine, this,
			mScheduleEngine));
		group.create_thread(boost::bind(&Game::RunEngines, this, mNetEngine, 
			mDataBaseEngine));
		
		// 所有线程运行，等待运行结束
		group.join_all();
	}
	else 
	{
		bool currentStillRunning = mIsStillRunning;
		boost::mutex::scoped_lock l(mStillRunningMutex);
		l.unlock();

		try
		{
			while (currentStillRunning)
			{
				mScheduleEngine->Frame();
				mNetEngine->Frame();
				mDataBaseEngine->Frame();

				l.lock();
				currentStillRunning = mIsStillRunning;
				l.unlock();
			}
		}
		catch (std::exception &e) 
		{
			mLog->Error("Handled exception in Game::run() : " +
				std::string(e.what()));
		} 
		catch (...)
		{
			mLog->Error("Handled unknown exception in Game::run()");
		}
	}
}
//----------------------------------------------------------------------------
void Game::RunEngine (Engine *e) 
{
	bool currentStillRunning = mIsStillRunning;
	try
	{
		while (currentStillRunning) 
		{
			e->Frame();
			currentStillRunning = mIsStillRunning;
		}
	} 
	catch (std::exception &e) 
	{
		mLog->Error("Handled exception in Game::RunEngine(Engine*) : " +
			std::string(e.what()));
	} 
	catch (...) 
	{
		mLog->Error("Handled unknown exception in Game::RunEngine(Engine*)");
	}
}
//----------------------------------------------------------------------------
void Game::RunEngines(Engine* e, Engine* e2) 
{
	bool currentStillRunning = mIsStillRunning;
	try 
	{
		while (currentStillRunning) 
		{
			e->Frame();
			e2->Frame();
			currentStillRunning = mIsStillRunning;
		}
	}
	catch (std::exception &e) 
	{
		mLog->Error(
			"Handled exception in Game::runEngines(Engine*, Engine*) : "
						+ std::string(e.what()));
	}
	catch (...) 
	{
		mLog->Error(
			"Handled unknown exception in Game::runEngines(Engine*, Engine*)");
	}
}
//----------------------------------------------------------------------------
void Game::Stop() 
{
	mLog->Info("Stop requested");

	boost::mutex::scoped_lock l(mStillRunningMutex);
	mIsStillRunning = false;
}
//----------------------------------------------------------------------------
std::vector<std::string> explode(std::string pStr, char pSrch) 
{
	std::vector<int> vctPos;
	std::vector<std::string> vct;
	unsigned int i;
	for (i = 0; i < pStr.size(); i++)
	{
		if (pStr.at(i) == pSrch)
			vctPos.push_back(i);
	}
	if (vctPos.size() > 0) 
	{
		vct.push_back(pStr.substr(0, vctPos.at(0)));
		for (i = 0; i < (vctPos.size() - 1); i++) 
		{
			vct.push_back(pStr.substr(vctPos.at(i) + 1, 
				vctPos.at(i + 1) - vctPos.at(i)));

			std::cout << "vctPos.at(" << i << ") : " << vctPos.at(i) << 
				std::endl;
		}
		vct.push_back(pStr.substr(vctPos.at(vctPos.size() - 1) + 1,
			pStr.size()));
	}
	else
	{
		vct.push_back(pStr);
	}

	return vct;
}
//----------------------------------------------------------------------------
std::string Game::Uptime() 
{
	return boost::posix_time::to_simple_string(
		boost::posix_time::second_clock::local_time() - mStartDate);
}
//----------------------------------------------------------------------------
bool Game::IsStillRunning() 
{
	boost::mutex::scoped_lock l(mStillRunningMutex);

	return mIsStillRunning;
}
//----------------------------------------------------------------------------
#ifdef UNIX
void SigintStop(int signum) 
{
	Game::GetInstance()->Stop();
}
#endif
//----------------------------------------------------------------------------
