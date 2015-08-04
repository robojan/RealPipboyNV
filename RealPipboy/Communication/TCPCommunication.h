#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <vector>

#include "Socket.h"
#include "../Packets/DataPacket.h"

typedef void *TCPClient;

class TCPCommunication
{
public:
	TCPCommunication(int port, std::string hostname = "0.0.0.0", int idleTimeOut = INFINITE, int maxConnections = 62);
	~TCPCommunication();

	void startListening();
	void stopListening();

	bool isConnected();

	int getNumClients();
	void getClients(std::vector<TCPClient> &clients);
	std::string getClientIP(TCPClient &client);
	void disconnectClient(TCPClient client);

	DataPacket *recvPacket(TCPClient *client = NULL);
	void sendPacket(DataPacket *packet, TCPClient client);
	void sendPacket(DataPacket *packet);

	int getIdleTimeout();

	void resetTxBytes();
	void resetRxBytes();
	size_t getTxBytes();
	size_t getRxBytes();

	void setMaxConnections(int max);
	int getMaxConnections();
private:
	static DWORD staticThreadHanlder(TCPCommunication *thisptr);
	DWORD threadHandler();
	void closeSockets();
	void processReceivedData(TCP::cSocket::cMemory *data, TCPClient client);

	int m_idleTimeout;
	std::string m_hostname;
	int m_port;
	TCP::cSocket m_TCPSocket;
	
	std::thread m_TCPThread;
	std::atomic<bool> m_TCPThreadActive;

	std::queue<std::pair<TCPClient, DataPacket *>> m_packetQueue;
	std::mutex m_packetQueueMutex;

	std::atomic<int> m_maxClients;

	char *m_sendBuffer;
	DWORD m_sendBufferSize;

	std::atomic<size_t> m_TxBytes;
	std::atomic<size_t> m_RxBytes;

	std::mutex m_sendMutex;
};

