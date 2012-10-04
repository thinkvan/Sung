//
// SNetMachine.h
//

#ifndef NETWORK_MACHINE_H
#define NETWORK_MACHINE_H

#include "SEngineEvent.h"
#include "SNetDevice.h"
#include "SNetEngine.h"
#include <boost/asio.hpp>
#include <list>

class TcpSocket;

class NetMachine : public NetDevice
{
public:
	NetMachine ();
	NetMachine (NetEngine*, TcpSocket*, int ID, int acctID, std::string, Log*);
	NetMachine (NetEngine*, int, std::string, Log*);

	bool HasAcctName (const std::string&);
	bool HasNick (const std::string&);

	void TcpSend(const EngineEvent&);
	
	inline int GetID() const { return mID; }
	inline boost::asio::io_service *GetIO() { return m_parent->getIO(); }
	
	inline int GetAcctID() { return mAcctId; }
	inline std::string GetAcctName() { return mAcctName; }
	
	int GetPlayerID ();
	std::string GetNickName ();
	
	void Move (EngineEvent e);
	void Move (float posX, float posY, float posZ, float pOW, float pOX, float pOY, float pOZ);
	void SaveMove ();
	
	void SetPlayer (Player player);
	
	Player *mPlayer;
	
	void Disconnect(const EngineEvent&);
	
	bool IsRunning();
	virtual void PushReceivedEvent (const EngineEvent&);
	
	void SetDeleting();
	inline bool Deleting() const { return m_deleting; }

private:
	NetEngine *mParent;
	TcpSocket *mTcpSocket;

	int mID;
	
	int mAcctID;
	std::string mAcctName;
	
	int mMoveCpt;
	
	bool mDeleting;
};

#endif
