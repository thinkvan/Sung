//
// SNetMachine.cpp
//

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/bind.hpp>
#include "SNetMachine.h"
#include "STcpSocket.h"

//----------------------------------------------------------------------------
NetworkMachine::NetworkMachine()
{
	m_tcpSocket = NULL;
	m_address = "";
	m_moveCpt = 0;
	m_deleting = false;
	
	networkLog = NULL;
}
//----------------------------------------------------------------------------
NetworkMachine::NetworkMachine(NetworkEngine* pP,
								TcpSocket *pSo,
								int pId,
								int pAcctId,
								std::string pAcctName,
								Log *engineLog)
								 : m_tcpSocket(pSo),
								 	m_parent(pP),
								 	m_id(pId),
								 	m_acctId(pAcctId),
								 	m_acctName(pAcctName) {
	
	networkLog = engineLog;
		networkLog->debug("Network Log correctly linked");
	
	m_player = NULL;
	m_moveCpt = 0;
	m_tcpSocket->setParent(this);
	m_address = m_tcpSocket->getSocket()->remote_endpoint().address().to_string();
	m_deleting = false;
	// set up the async reading
	m_tcpSocket->setBind();
}
//----------------------------------------------------------------------------
/**
 * another constructor
 *\param pP pointor to parent
 *\param pI id for this network machine
 *\param pAcctName name of the account linked with the machine
 */
NetworkMachine::NetworkMachine(NetworkEngine* pP,
								int pI,
								std::string pAcctName,
								Log *engineLog)
								 : m_acctName(pAcctName) {
	
	networkLog = engineLog;
		networkLog->debug("Network Log correctly linked");
	
	m_parent = pP;
	m_id = pI;
	m_moveCpt = 0;
}
//----------------------------------------------------------------------------
bool NetworkMachine::hasAcctName(const std::string& pN) {
	if (m_deleting)
		return false;
	return (pN == m_acctName);
}
//----------------------------------------------------------------------------
bool NetworkMachine::hasNick(const std::string& pN) {
	return (pN == getNick());
}
//----------------------------------------------------------------------------
void NetworkMachine::tcpSend(const EngineEvent& pE) {
	if (!m_tcpSocket->sendEvent(pE)) {
		if (m_player)
			networkLog->debug(getAcctName()
							 + " ("
							 + getNick()
							 + ") disconnected on "
							 + m_address);
		else
			networkLog->debug(getAcctName()
							 + " disconnected on "
							 + m_address);
		m_parent->removeMachine(this);
	}
}
//----------------------------------------------------------------------------
void NetworkMachine::disconnect(const EngineEvent& pE) {
	if (m_player)
		saveMove();
	tcpSend(pE);
	m_parent->removeMachine(this);
}
//----------------------------------------------------------------------------
std::string NetworkMachine::getNick() {
	if (m_player)
		return m_player->getNick();
	else
		return "";
}
//----------------------------------------------------------------------------
int NetworkMachine::getPlayerId() {
	if (m_player)
		return m_player->getId();
	else
		return 1;
}
//----------------------------------------------------------------------------
void NetworkMachine::setPlayer(Player pPlayer) {
	m_player = new Player(pPlayer);
}
//----------------------------------------------------------------------------
void NetworkMachine::move(EngineEvent pE) {
	if (pE.m_type != EngineEvent::PLAYER_POS && pE.m_type != EngineEvent::PLAYER_PROP)
		return;

	m_player->move(pE);
	m_moveCpt++;
	if (m_moveCpt >= 50)			//TODO : Much less frequent, test value
		saveMove();
}
//----------------------------------------------------------------------------
void NetworkMachine::move(float pPosX, float pPosY, float pPosZ, float pOW, float pOX, float pOY, float pOZ) {
	m_player->move(pPosX, pPosY, pPosZ, pOW, pOX, pOY, pOZ);
	m_moveCpt++;
	if (m_moveCpt >= 50)			//TODO : Much less frequent, test value
		saveMove();
}
//----------------------------------------------------------------------------
void NetworkMachine::saveMove() {
	if (m_player) {
		EngineEvent e;
		e = m_player->getPos();
		e.m_iData["PLAYER_ID"] = getPlayerId();
		m_parent->sendMessageToDb(e);
	}
	m_moveCpt = 0;
}
//----------------------------------------------------------------------------
bool NetworkMachine::isRunning() {
	return m_parent->isRunning();
}
//----------------------------------------------------------------------------
void NetworkMachine::pushReceivedEvent(const EngineEvent& pE) {
	EngineEvent e = pE;
	e.m_iData["ID"] = m_id;
	e.m_iData["ACCT_ID"] = m_acctId;
	if (m_player)
		e.m_iData["PLAYER_ID"] = getPlayerId();
	if (e.m_type == EngineEvent::CHAT_EVENT)
		e.m_sData["PLAYER_NICK"] = getNick();
	m_parent->pushReceivedEvent(e);
}
//----------------------------------------------------------------------------
void NetworkMachine::setDeleting() {
	m_deleting = true;
}
//----------------------------------------------------------------------------