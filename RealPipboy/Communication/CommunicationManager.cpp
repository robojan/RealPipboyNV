#include "CommunicationManager.h"

#include "common/IPrefix.h"

CommunicationManager CommunicationManager::m_ourInstance;

CommunicationManager::CommunicationManager() :
	m_tcp(NULL)
{
	m_tcp = new TCPCommunication(28115, "0.0.0.0", -1, 1);
}

CommunicationManager::~CommunicationManager()
{
	if (m_tcp != NULL) {
		delete m_tcp;
		m_tcp = NULL;
	}
}


CommunicationManager & CommunicationManager::getInstance()
{
	return m_ourInstance;
}

void CommunicationManager::setTCPSettings(const char *hostname, int port)
{
	if (m_tcp != NULL) {
		stopListening();
		delete m_tcp;
		m_tcp = NULL;
	}
	m_tcp = new TCPCommunication(port, hostname, -1, 1);
}

void CommunicationManager::startListening()
{
	ASSERT(m_tcp != NULL);
	m_tcp->startListening();
}

void CommunicationManager::stopListening()
{
	ASSERT(m_tcp != NULL);
	m_tcp->stopListening();
}

void CommunicationManager::setIdleTimeout(int time)
{
	ASSERT(m_tcp != NULL);
	ASSERT(false);
}

int CommunicationManager::getIdleTimeout()
{
	ASSERT(m_tcp != NULL);
	return m_tcp->getIdleTimeout();
}

bool CommunicationManager::isClientConnected()
{
	ASSERT(m_tcp != NULL);
	return m_tcp->isConnected() && m_tcp->getNumClients() > 0;
}

std::string CommunicationManager::getClientName()
{
	return "Unimplemented";
}

void CommunicationManager::registerPacketHandler(MessageType type, IPacketHandler *handler)
{
	ASSERT(handler != NULL);
	std::map<MessageType, std::set<IPacketHandler *>>::iterator it = m_packetHandlers.find(type);
	if (it == m_packetHandlers.end()) {
		m_packetHandlers[type] = std::set<IPacketHandler *>();
		it = m_packetHandlers.find(type);
	}
	it->second.insert(handler);
}

void CommunicationManager::deregisterPacketHandler(MessageType type, IPacketHandler *handler)
{
	std::map<MessageType, std::set<IPacketHandler *>>::iterator it = m_packetHandlers.find(type);
	if (it == m_packetHandlers.end()) {
		return;
	}
	it->second.erase(handler);
}

void CommunicationManager::sendPacket(DataPacket *packet, bool doDelete/* = true*/)
{
	if (!isClientConnected())
		return;
	m_tcp->sendPacket(packet);
	if (doDelete){
		delete packet;
	}	
}

void CommunicationManager::update()
{
	if (m_tcp->isConnected()) {
		DataPacket *packet;
		while ((packet = m_tcp->recvPacket()) != NULL) {
			MessageType type = packet->getType();
			std::map<MessageType, std::set<IPacketHandler *>>::iterator it;
			it = m_packetHandlers.find(type);
			if (it != m_packetHandlers.end()) {
				for (std::set<IPacketHandler *>::iterator handler = it->second.begin();
					handler != it->second.end(); ++handler) {
					(*handler)->handlePacket(packet);
				}
			}
			else {
				_WARNING("Could not find packet handler for packet type: %d", (int)type);
			}
			delete packet;
		}
	}	
}
