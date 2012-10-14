//
// SDataBaseEngine.h
//

#ifndef SDATABASEENGINE_H
#define SDATABASEENGINE_H

#include "SGame.h"
#include "SEngine.h"
#include "SPlayer.h"
#include "SNPC.h"

namespace Sung
{

	class DataBaseEngine : public Engine 
	{
	public:
		DataBaseEngine (Game *game, int logPriority, int logOutput);
		~DataBaseEngine ();
		void Frame () { ProcessQueue(); }

		Player GetPlayerByID (int ID, bool &internal, int acctId = -1);
		EngineEvent GetPlayerPos (int ID);

		NPC GetNPCByID (int ID);
		EngineEvent getNpcPos(int pId);

	private:
		virtual void ProcessNewPlayer(const EngineEvent& pE);
		virtual void ProcessDelPlayer(const EngineEvent& pE);
		virtual void ProcessWelcome(const EngineEvent& pE);
		virtual void ProcessRemoveMachine(const EngineEvent& pE);
		virtual void ProcessCharactersList(const EngineEvent& pE);
		virtual void ProcessCreateCharacter(const EngineEvent& pE);
		virtual void ProcessDeleteCharacter(const EngineEvent& pE);
		virtual void ProcessPlayerPos(const EngineEvent& pE);
		virtual void ProcessPlayerProp(const EngineEvent& pE);
		virtual void ProcessNpcProp(const EngineEvent& pE);
		virtual void ProcessPing(const EngineEvent& pE);
		virtual void ProcessEnvironmentInfo(const EngineEvent& pE);
		virtual void ProcessAuth(const EngineEvent& pE);
		void SendNPCInfo (int machineId, int mapID);

		DataBase* mDataBase;
		bool mIsDBOK;
	};

}

#endif