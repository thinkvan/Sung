//
// SNetEngine.cpp
//

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/bind.hpp>
#include "SNetEngine.h"
#include "SNetMachine.h"

#include "network_machine.h"
#include "network_console.h"
#include "tcp_socket.h"
#include "../config_file_parser.h"
//#include "../DB/db_engine.h"

#include "../convert.h"

#include "../../Log/log.h"

/**
 * constructor. read config and initialize ids
 *\param pG game engine to link with
 */
NetworkEngine::NetworkEngine(Game* pG, int log_priority, int log_output) : Engine(pG, log_priority, log_output) {
	
	engineLog = new Log("Network", log_priority, log_output);
	
	m_tcpAcceptorCons = NULL;
	m_tcpAcceptor = NULL;
	m_running = true;
	m_nextConsoleId = 1;
	m_nextMachineId = 1;
	m_io = new boost::asio::io_service;
	try {
		getConfigData();
	} catch(std::exception&e) {
		engineLog->error("Handled exception in NetworkEngine::NetworkEngine() : " + std::string(e.what()));
	} catch(std::string& e) {
		// cannot create network, will continue without networking, no listening, no connect
		engineLog->error("Handled exception in NetworkEngine::NetworkEngine() : " + e);
		m_running = false;
	} catch(...) {
		engineLog->error("Handled unknown exception in NetworkEngine::NetworkEngine()");
	}
}
