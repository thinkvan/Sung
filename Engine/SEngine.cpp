//
// SEngine.cpp
//

#include "SEngine.h"
using namespace Sung;

//----------------------------------------------------------------------------
Engine::Engine(Game *game, int logPriority, int logOutput)
:
mParent(game) 
{
	mNetEngine = 0;
	mDataBaseEngine = 0;
	mEnvironmentEngine = 0;
	mScheduleEngine = 0;	
	mLog = 0;
}
//----------------------------------------------------------------------------
Engine::~Engine()
{
}
//----------------------------------------------------------------------------
void Engine::ProcessQueue()
{
	while (!mEventsQueue.empty()) 
	{
		boost::mutex::scoped_lock lock(mQueueMutex);

		EngineEvent ent = mEventsQueue.front();
		mEventsQueue.pop();
		
		lock.unlock();

		ProcessEvent(ent);
	}
}
//----------------------------------------------------------------------------
void Engine::ProcessEvent(const EngineEvent &ent) 
{
	static struct 
	{ 
		EngineEvent::EEType t;
		void (Engine::*f)(const EngineEvent&);
	} tab[] =
	{
		{EngineEvent::MSG_NICK, &Engine::ProcessMsgNick},
		{EngineEvent::CHARACTERS_LIST, &Engine::ProcessCharactersList},
		{EngineEvent::GAME_PROP, &Engine::ProcessGameProp},
		{EngineEvent::WELCOME, &Engine::ProcessWelcome},
		{EngineEvent::REFUSED, &Engine::ProcessRefused},
		{EngineEvent::NEW_PLAYER, &Engine::ProcessNewPlayer},
		{EngineEvent::RUN_MULTI, &Engine::ProcessRunMulti},
		{EngineEvent::SEND_GAME_PROP, &Engine::ProcessSendGameProp},
		{EngineEvent::DEL_PLAYER, &Engine::ProcessDelPlayer},
		{EngineEvent::LOAD, &Engine::ProcessLoad},
		{EngineEvent::ADD_NODE, &Engine::ProcessAddNode},
		{EngineEvent::REM_NODE, &Engine::ProcessRemNode},
		{EngineEvent::MOVE_NODE, &Engine::ProcessMoveNode},
		{EngineEvent::REMOVE_MACHINE, &Engine::ProcessRemoveMachine},
		{EngineEvent::REMOVE_CONSOLE, &Engine::ProcessRemoveConsole},
		{EngineEvent::PLAYER_POS, &Engine::ProcessPlayerPos},
		{EngineEvent::NPC_POS, &Engine::ProcessNpcPos},
		{EngineEvent::CREATE_CHARACTER, &Engine::ProcessCreateCharacter},
		{EngineEvent::DELETE_CHARACTER, &Engine::ProcessDeleteCharacter},
		{EngineEvent::ITEM_PROP, &Engine::ProcessItemProp},
		{EngineEvent::CHAT_EVENT, &Engine::ProcessChatEvent},
		{EngineEvent::PING, &Engine::ProcessPing},
		{EngineEvent::TARGET_CHANGED, &Engine::ProcessTargetChanged},
		{EngineEvent::PLAYER_PROP, &Engine::ProcessPlayerProp},
		{EngineEvent::NPC_PROP, &Engine::ProcessNpcProp},
		{EngineEvent::CONSOLE_COMMAND, &Engine::ProcessConsoleCommand},
		{EngineEvent::ENVIRONMENT_INFO, &Engine::ProcessEnvironmentInfo},
		{EngineEvent::PLAY_SOUND, &Engine::ProcessPlaySound},
		{EngineEvent::AUTH, &Engine::ProcessAuth}
	};
	(this->*tab[ent.mType].f)(ent);
}
//----------------------------------------------------------------------------
void Engine::PushEvent(const EngineEvent &ent) 
{
	boost::mutex::scoped_lock lock(mQueueMutex);

	mEventsQueue.push(ent);
}
//----------------------------------------------------------------------------
void Engine::SendMessageToNetwork (const EngineEvent &ent)
{
	mNetEngine->PushEvent(ent);
}
//----------------------------------------------------------------------------
void Engine::SendMessageToDataBase(const EngineEvent &ent) 
{
	mDataBaseEngine->PushEvent(ent);
}
//----------------------------------------------------------------------------
void Engine::SendMessageToEnvironment(const EngineEvent &ent)
{
	mEnvironmentEngine->PushEvent(ent);
}
//----------------------------------------------------------------------------
void Engine::SendMessageToSchedule(const EngineEvent &ent) 
{
	mScheduleEngine->PushEvent(ent);
}
//----------------------------------------------------------------------------
