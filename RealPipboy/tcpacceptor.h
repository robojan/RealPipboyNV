#ifndef TCP_ACCEPTOR_H_
#define TCP_ACCEPTOR_H_

#include <string>
#include <mutex>
#include <queue>
#include <thread>
#include "Semaphore.h"

#include <WinSock2.h>

class TCPStream;

class TCPAcceptor
{
public:
	TCPAcceptor(const std::string &bindAddr, int port, int maxConnections = 1);
	TCPAcceptor(const std::string &bindAddr, const std::string &port, int maxConnections = 1);
	~TCPAcceptor();

	bool startListening();
	void stopListening();
	TCPStream *getConnection(bool block = false);

private:
	static unsigned int __cdecl staticListenThread(void *lpParam);
	unsigned int listenThread();

	int m_maxConnections;
	std::string m_bindAddr;
	std::string m_port;

	std::mutex m_listenSocketMutex; 
	SOCKET m_listenSocket;
	bool m_listening;
	std::thread m_listeningThread;

	Semaphore m_clientConnectionsSemaphore;
	std::mutex m_clientConnectionsMutex;
	std::queue<TCPStream *> m_clientConnections;
};

#endif