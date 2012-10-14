//
// SEngine.h
//

#ifndef SENGINE_H
#define SENGINE_H

#include <queue>
#include <map>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/asio.hpp>
#include "SEngineEvent.h"
#include "SLog.h"

namespace Sung
{

	class NetEngine;
	class DataBaseEngine;
	class ScheduleEngine;
	class Game;

	class Engine 
	{
	public:
		Engine (Game *game, int logPriority, int logOutput);
		virtual ~Engine();

		void PushEvent(const EngineEvent&);
		void ProcessQueue();

		virtual void Frame() = 0;

		void AttachNetEngine (NetEngine *engine) { mNetEngine = engine; }
		void AttachDataBaseEngine (DataBaseEngine *engine) { mDataBaseEngine = engine; }
		void AttachEnvironmentEngine(EnvironmentEngine *engine){ mEnvironmentEngine = engine;}
		void AttachScheduleEngine (ScheduleEngine *engine) { mScheduleEngine = engine; }

		void SendMessageToNet (const EngineEvent &ent);
		void SendMessageToDataBase (const EngineEvent &ent);
		void SendMessageToEnvironment (const EngineEvent &ent);
		void SendMessageToSchedule (const EngineEvent &ent);

		Game* GetParent() { return mParent; }
		NetEngine *GetNetEngine() { return mNetworkEngine; }
		DataBaseEngine *getDataBaseEngine() {return mDataBaseEngine;}
		EnvironmentEngine *GetEnvironmentEngine() { return mEnvironmentEngine; }
		ScheduleEngine *GetScheduleEngine() { return mScheduleEngine; }

	protected:
		void ProcessEvent(const EngineEvent&);
		virtual void ProcessMsgNick(const EngineEvent&) {};
		virtual void ProcessCharactersList(const EngineEvent&) {};
		virtual void ProcessGameProp(const EngineEvent&) {};
		virtual void ProcessWelcome(const EngineEvent&) {};
		virtual void ProcessRefused(const EngineEvent&) {};
		virtual void ProcessNewPlayer(const EngineEvent&) {};
		virtual void ProcessRunMulti(const EngineEvent&) {};
		virtual void ProcessSendGameProp(const EngineEvent&) {};
		virtual void ProcessDelPlayer(const EngineEvent&) {};
		virtual void ProcessLoad(const EngineEvent&) {};
		virtual void ProcessAddNode(const EngineEvent&) {};
		virtual void ProcessRemNode(const EngineEvent&) {};
		virtual void ProcessMoveNode(const EngineEvent&) {};
		virtual void ProcessRemoveMachine(const EngineEvent&) {};
		virtual void ProcessRemoveConsole(const EngineEvent&) {};
		virtual void ProcessPlayerPos(const EngineEvent&) {};
		virtual void ProcessNpcPos(const EngineEvent&) {};
		virtual void ProcessCreateCharacter(const EngineEvent&) {};
		virtual void ProcessDeleteCharacter(const EngineEvent&) {};
		virtual void ProcessItemProp(const EngineEvent&) {};
		virtual void ProcessChatEvent(const EngineEvent&) {};
		virtual void ProcessPing(const EngineEvent&) {}
		virtual void ProcessTargetChanged(const EngineEvent&) {};
		virtual void ProcessPlayerProp(const EngineEvent&) {};
		virtual void ProcessNpcProp(const EngineEvent&) {};
		virtual void ProcessConsoleCommand(const EngineEvent&) {};
		virtual void ProcessEnvironmentInfo(const EngineEvent&) {};
		virtual void ProcessPlaySound(const EngineEvent&) {};
		virtual void ProcessAuth(const EngineEvent&) {};

		Game *mParent;
		Log *mLog;	
		NetEngine *mNetEngine;
		DataBaseEngine *mDataBaseEngine;
		ScheduleEngine *mScheduleEngine;
		boost::mutex mQueueMutex;
		std::queue<EngineEvent> mEventsQueue;
	};

}

#endif