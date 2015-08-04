#pragma once

#include "TCPCommunication.h"
#include "../Packets/packets.h"

#include <string>
#include <map>
#include <set>

class IPacketHandler {
public:
	virtual void handlePacket(DataPacket *packet) = 0;
};

class CommunicationManager
{
public:
	static CommunicationManager &getInstance();

	void startListening();
	void stopListening();

	void setIdleTimeout(int time);
	int getIdleTimeout();

	bool isClientConnected();
	std::string getClientName();
	
	void registerPacketHandler(MessageType type, IPacketHandler *handler);
	void deregisterPacketHandler(MessageType type, IPacketHandler *handler);

	void sendPacket(DataPacket *packet, bool doDelete = true);

	void update();

private:
	CommunicationManager();
	~CommunicationManager();
	
	TCPCommunication *m_tcp;
	std::map<MessageType, std::set<IPacketHandler *>> m_packetHandlers;

	static CommunicationManager m_ourInstance;
};

