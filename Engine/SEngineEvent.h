//
// SEngineEvent.h
//

#ifndef SENGINEEVENT_H
#define SENGINEEVENT_H

#include <iostream>
#include <map>
#include <string>

#include "SLog.h"
#include "SConvert.h"

namespace Sung
{

	class EngineEvent
	{
	public:
		enum EEType 
		{
			MSG_NICK,
			CHARACTERS_LIST,
			GAME_PROP,
			WELCOME,
			REFUSED,
			NEW_PLAYER,
			RUN_MULTI,
			SEND_GAME_PROP,
			DEL_PLAYER,
			LOAD,
			ADD_NODE,
			REM_NODE,
			MOVE_NODE,
			REMOVE_MACHINE,
			REMOVE_CONSOLE,
			PLAYER_POS,
			NPC_POS,
			CREATE_CHARACTER,
			DELETE_CHARACTER,
			ITEM_PROP,
			CHAT_EVENT,
			PING,
			TARGET_CHANGED,
			PLAYER_PROP,
			NPC_PROP,
			CONSOLE_COMMAND,
			ENVIRONMENT_INFO,
			PLAY_SOUND,
			AUTH,

		};

		EEType mType;
		std::map<std::string, std::string> msData;
		std::map<std::string, int> miData;
		std::map<std::string, float> mfData;

		bool operator == (const EngineEvent& e) 
		{
			return (mType == e.mType) && (msData == e.msData) && 
				(miData == e.miData) && (mfData == e.mfData);
		}

		void DisplayOnLog (Log *engineLog) const 
		{
			std::string display = "Type : ";

			switch (mType) 
			{
			case MSG_NICK:
				display += "MSG_NICK";
				break;
			case CHARACTERS_LIST:
				display += "CHARACTERS_LIST";
				break;
			case GAME_PROP:
				display +="GAME_PROP";
				break;
			case WELCOME:
				display += "WELCOME";
				break;
			case REFUSED:
				display += "REFUSED";
				break;
			case NEW_PLAYER:
				display += "NEW_PLAYER";
				break;
			case RUN_MULTI:
				display += "RUN_MULTI";
				break;
			case SEND_GAME_PROP:
				display += "SEND_GAME_PROP";
				break;
			case DEL_PLAYER:
				display += "DEL_PLAYER";
				break;
			case LOAD:
				display += "LOAD";
				break;
			case ADD_NODE:
				display += "ADD_NODE";
				break;
			case REM_NODE:
				display += "REM_NODE";
				break;
			case MOVE_NODE:
				display +="MOVE_NODE";
				break;
			case REMOVE_MACHINE:
				display += "REMOVE_MACHINE";
				break;
			case REMOVE_CONSOLE:
				display += "REMOVE_CONSOLE";
				break;
			case PLAYER_POS:
				display += "PLAYER_POS";
				break;
			case NPC_POS:
				display += "NPC_POS";
				break;
			case CREATE_CHARACTER:
				display += "CREATE_CHARACTER";
				break;
			case DELETE_CHARACTER:
				display += "DELETE_CHARACTER";
				break;
			case ITEM_PROP:
				display += "ITEM_PROP";
				break;
			case CHAT_EVENT:
				display += "CHAT_EVENT";
				break;
			case PING:
				display += "PING";
				break;
			case TARGET_CHANGED:
				display += "TARGET_CHANGED";
				break;
			case PLAYER_PROP:
				display += "PLAYER_PROP";
				break;
			case CONSOLE_COMMAND:
				display += "CONSOLE_COMMAND";
				break;
			case ENVIRONMENT_INFO:
				display += "ENVIRONMENT_INFO";
				break;
			case PLAY_SOUND:
				display += "PLAY_SOUND";
				break;
			default:
				display += "UNKNOWN";
				break;
			}
			engineLog->Debug(display);

			for (std::map<std::string, std::string>::const_iterator it = msData.begin(); it != msData.end(); it++)
				engineLog->Debug(it->first
				+ " : "
				+ it->second);
			for (std::map<std::string, int>::const_iterator it = miData.begin(); it != miData.end(); it++)
				engineLog->Debug(it->first
				+ " : "
				+ stringify<int>(it->second));
			for (std::map<std::string, float>::const_iterator it = mfData.begin(); it != mfData.end(); it++)
				engineLog->Debug(it->first
				+ " : "
				+ stringify<float>(it->second));
		}

		void Display() const
		{
			std::cout << "Type : ";

			switch (mType) 
			{
			case MSG_NICK:
				std::cout << "MSG_NICK";
				break;
			case CHARACTERS_LIST:
				std::cout << "CHARACTERS_LIST";
				break;
			case GAME_PROP:
				std::cout << "GAME_PROP";
				break;
			case WELCOME:
				std::cout << "WELCOME";
				break;
			case REFUSED:
				std::cout << "REFUSED";
				break;
			case NEW_PLAYER:
				std::cout << "NEW_PLAYER";
				break;
			case RUN_MULTI:
				std::cout << "RUN_MULTI";
				break;
			case SEND_GAME_PROP:
				std::cout << "SEND_GAME_PROP";
				break;
			case DEL_PLAYER:
				std::cout << "DEL_PLAYER";
				break;
			case LOAD:
				std::cout << "LOAD";
				break;
			case ADD_NODE:
				std::cout << "ADD_NODE";
				break;
			case REM_NODE:
				std::cout << "REM_NODE";
				break;
			case MOVE_NODE:
				std::cout << "MOVE_NODE";
				break;
			case REMOVE_MACHINE:
				std::cout << "REMOVE_MACHINE";
				break;
			case REMOVE_CONSOLE:
				std::cout << "REMOVE_CONSOLE";
				break;
			case PLAYER_POS:
				std::cout << "PLAYER_POS";
				break;
			case NPC_POS:
				std::cout << "NPC_POS";
				break;
			case CREATE_CHARACTER:
				std::cout << "CREATE_CHARACTER";
				break;
			case DELETE_CHARACTER:
				std::cout << "DELETE_CHARACTER";
				break;
			case ITEM_PROP:
				std::cout << "ITEM_PROP";
				break;
			case CHAT_EVENT:
				std::cout << "CHAT_EVENT";
				break;
			case PING:
				std::cout << "PING";
				break;
			case TARGET_CHANGED:
				std::cout << "TARGET_CHANGED";
				break;
			case PLAYER_PROP:
				std::cout << "PLAYER_PROP";
				break;
			case CONSOLE_COMMAND:
				std::cout << "CONSOLE_COMMAND";
				break;
			case ENVIRONMENT_INFO:
				std::cout << "ENVIRONMENT_INFO";
				break;
			case PLAY_SOUND:
				std::cout << "PLAY_SOUND";
				break;
			default:
				std::cout << "UNKNOWN";
				break;
			}
			std::cout << std::endl;

			for (std::map<std::string, std::string>::const_iterator it = msData.begin(); it != msData.end(); it++)
				std::cout << it->first << " : " << it->second << std::endl;
			for (std::map<std::string, int>::const_iterator it = miData.begin(); it != miData.end(); it++)
				std::cout << it->first << " : " << it->second << std::endl;
			for (std::map<std::string, float>::const_iterator it = mfData.begin(); it != mfData.end(); it++)
				std::cout << it->first << " : " << it->second << std::endl;
		}

		template<class Archive>
		void serialize (Archive& ar, const unsigned int) 
		{
			ar & mType;
			ar & msData;
			ar & miData;
			ar & mfData;
		}
	};

}

#endif