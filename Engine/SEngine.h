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

#include "engine_event.h"

#include "../Log/log.h"

//class GameEngine;
class NetEngine;
class DbEngine;
class EnvironmentEngine;
class ScheduleEngine;
//class NetworkProcessingEngine;
class Game;

class Engine {
public:
	Engine(Game*, int log_priority, int log_output);
	virtual ~Engine();

	void PushEvent(const EngineEvent&);
	void processQueue();

	virtual void frame() = 0;

	//	void attachGameEngine(GameEngine *pE){m_gameEngine = pE;}
	void attachNetworkEngine(NetEngine *pE){m_networkEngine = pE;}
	void attachDbEngine(DbEngine *pE){m_dbEngine = pE;}
	void attachEnvironmentEngine(EnvironmentEngine *pE){m_environmentEngine = pE;}
	void attachScheduleEngine(ScheduleEngine *pE){m_scheduleEngine = pE;}
	//	void attachNetworkProcessingEngine(NetworkProcessingEngine *pE){m_networkProcessingEngine = pE;}

	//	void sendMessageToGame(const EngineEvent&);
	void sendMessageToNetwork(const EngineEvent&);
	void sendMessageToDb(const EngineEvent&);
	void sendMessageToEnvironment(const EngineEvent&);
	void sendMessageToSchedule(const EngineEvent&);
	//	void sendMessageToNetworkProcessing(const EngineEvent&);

	inline Game*					getParent()						{return m_parent;}
	//	inline GameEngine*				getGameEngine()					{return m_gameEngine;}
	inline NetEngine*			getNetworkEngine()				{return m_networkEngine;}
	inline DbEngine* 				getDbEngine()					{return m_dbEngine;}
	inline EnvironmentEngine*		getEnvironmentEngine()			{return m_environmentEngine;}
	inline ScheduleEngine*			getScheduleEngine()				{return m_scheduleEngine;}
	//	inline NetworkProcessingEngine*	getNetworkProcessingEngine()	{return m_networkProcessingEngine;}

protected:
	virtual void processMsgNick(const EngineEvent&) {};
	virtual void processCharactersList(const EngineEvent&) {};
	virtual void processGameProp(const EngineEvent&) {};
	virtual void processWelcome(const EngineEvent&) {};
	virtual void processRefused(const EngineEvent&) {};
	virtual void processNewPlayer(const EngineEvent&) {};
	virtual void processRunMulti(const EngineEvent&) {};
	virtual void processSendGameProp(const EngineEvent&) {};
	virtual void processDelPlayer(const EngineEvent&) {};
	virtual void processLoad(const EngineEvent&) {};
	virtual void processAddNode(const EngineEvent&) {};
	virtual void processRemNode(const EngineEvent&) {};
	virtual void processMoveNode(const EngineEvent&) {};
	virtual void processRemoveMachine(const EngineEvent&) {};
	virtual void processRemoveConsole(const EngineEvent&) {};
	virtual void processPlayerPos(const EngineEvent&) {};
	virtual void processNpcPos(const EngineEvent&) {};
	virtual void processCreateCharacter(const EngineEvent&) {};
	virtual void processDeleteCharacter(const EngineEvent&) {};
	virtual void processItemProp(const EngineEvent&) {};
	virtual void processChatEvent(const EngineEvent&) {};
	virtual void processPing(const EngineEvent&) {}
	virtual void processTargetChanged(const EngineEvent&) {};
	virtual void processPlayerProp(const EngineEvent&) {};
	virtual void processNpcProp(const EngineEvent&) {};
	virtual void processConsoleCommand(const EngineEvent&) {};
	virtual void processEnvironmentInfo(const EngineEvent&) {};
	virtual void processPlaySound(const EngineEvent&) {};
	virtual void processAuth(const EngineEvent&) {};

	Game *m_parent;

	Log *engineLog;

	//	GameEngine *m_gameEngine;
	NetEngine *m_networkEngine;
	DbEngine *m_dbEngine;
	EnvironmentEngine *m_environmentEngine;
	ScheduleEngine *m_scheduleEngine;
	//	NetworkProcessingEngine *m_networkProcessingEngine;

	// mutex to protect the event queue
	boost::mutex m_queueMutex;

	std::queue<EngineEvent> m_eventsQueue;
	void processEvent(const EngineEvent&);
};

#endif