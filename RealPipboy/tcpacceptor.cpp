#include "tcpacceptor.h"
#include "tcpstream.h"

#include "common/IPrefix.h"

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <cstddef>
#include <thread>

TCPAcceptor::TCPAcceptor(const std::string &bindAddr, int port, int maxConnections /*= 1*/)
	: m_bindAddr(bindAddr), m_maxConnections(maxConnections),
	m_listenSocket(INVALID_SOCKET), m_listening(false),
	m_clientConnectionsSemaphore(m_maxConnections)
{
	m_port = std::to_string(port);
}

TCPAcceptor::TCPAcceptor(const std::string &bindAddr, const std::string &port, 
						 int maxConnections /*= 1*/)
	: m_bindAddr(bindAddr), m_maxConnections(maxConnections), m_port(port), 
	m_listenSocket(INVALID_SOCKET), m_listening(false),
	m_clientConnectionsSemaphore(m_maxConnections)
{

}

TCPAcceptor::~TCPAcceptor()
{
}

bool TCPAcceptor::startListening()
{
	struct addrinfo hints;
	struct addrinfo *addrInfo = NULL;

	if (m_listening) {
		_WARNING("A thread is already listening", m_listening);
		return false;
	}

	_MESSAGE("Start listening on port %s:%s", m_bindAddr.c_str(), m_port.c_str());

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	int iResult = getaddrinfo(m_bindAddr.empty() ? NULL : m_bindAddr.c_str(), m_port.c_str(), 
							  &hints, &addrInfo);
	if (iResult != 0) {
		_ERROR("getaddrinfo failed with error: %08X", iResult);
		return false;
	}

	// Create an socket
	m_listenSocketMutex.lock();
	m_listenSocket = socket(addrInfo->ai_family, addrInfo->ai_socktype, addrInfo->ai_protocol);
	if (m_listenSocket == INVALID_SOCKET) {
		_ERROR("socket failed with error: %ld", WSAGetLastError());
		m_listenSocketMutex.unlock();
		freeaddrinfo(addrInfo);
		return false;
	}

	// Bind to the socket
	iResult = bind(m_listenSocket, addrInfo->ai_addr, (int) addrInfo->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		_ERROR("bind failed with error: %ld", WSAGetLastError());
		m_listenSocketMutex.unlock();
		freeaddrinfo(addrInfo);
		closesocket(m_listenSocket);
		return false;
	}

	freeaddrinfo(addrInfo);

	// Setup TCP listening socket
	iResult = listen(m_listenSocket, m_maxConnections);
	if (iResult == SOCKET_ERROR) {
		_ERROR("listen failed with error: %ld", WSAGetLastError());
		m_listenSocketMutex.unlock();
		closesocket(m_listenSocket);
		return false;
	}
	m_listenSocketMutex.unlock();


	// start the listen thread
	m_listening = true;
	m_listeningThread = std::thread(staticListenThread, this);

	return true;
}

void TCPAcceptor::stopListening()
{
	if (m_listenSocket != INVALID_SOCKET) {
		m_listening = false;
		closesocket(m_listenSocket);
		m_listeningThread.join();
	}
}

TCPStream * TCPAcceptor::getConnection(bool block /*= false*/)
{
	if (!block) {
		m_clientConnectionsMutex.lock();
		int size = m_clientConnections.size();
		m_clientConnectionsMutex.unlock();
		if (size == 0) {
			return NULL;
		}
	}
	m_clientConnectionsSemaphore.notify();
	m_clientConnectionsMutex.lock();
	TCPStream *stream = m_clientConnections.front();
	m_clientConnections.pop();
	m_clientConnectionsMutex.unlock();
	return stream;
}

unsigned int __cdecl TCPAcceptor::staticListenThread(void *lpParam)
{
	TCPAcceptor *acceptor = reinterpret_cast<TCPAcceptor *>(lpParam);
	return acceptor->listenThread();
}

unsigned int TCPAcceptor::listenThread()
{
	SOCKET clientSocket = INVALID_SOCKET;
	struct sockaddr clientAddr;
	int addrLen = sizeof(clientAddr);
	ZeroMemory(&clientAddr, sizeof(clientAddr));
	WCHAR addressString[50];
	ZeroMemory(&addressString, 50);
	DWORD addressStringSize = sizeof(addressString) / sizeof(addressString[0]);
	while (m_listening) {
		m_listenSocketMutex.lock();
		clientSocket = accept(m_listenSocket, (sockaddr *) &clientAddr, &addrLen);
		m_listenSocketMutex.unlock();
		if (clientSocket == INVALID_SOCKET) {
			_ERROR("accept failed with error: %ld\n", WSAGetLastError());
			continue;
		}
		if (WSAAddressToStringW(&clientAddr, addrLen, NULL, addressString, &addressStringSize) == 0){
			_MESSAGE("Client connected: %ls", addressString);
		}
		else {
			_MESSAGE("Client connected");
		}
		
		m_clientConnectionsSemaphore.wait();
		m_clientConnectionsMutex.lock();
		m_clientConnections.push(new TCPStream(clientSocket));
		m_clientConnectionsMutex.unlock();
	}
	m_listenSocketMutex.lock();
	closesocket(m_listenSocket);
	m_listenSocket = INVALID_SOCKET;
	m_listenSocketMutex.unlock();
	return 0;
}
