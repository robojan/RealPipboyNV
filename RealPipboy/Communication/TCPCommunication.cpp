#include "TCPCommunication.h"

#include "common/IPrefix.h"

#include <WS2tcpip.h>
#include <cstdio>
#include <cassert>

#include "../Packets/packets.h"

TCPCommunication::TCPCommunication(int port, std::string hostname /* = "0.0.0.0" */, 
	int idleTimeOut /* = INFINTE */, int maxConnections /*= 62*/) :
								   m_port(port), m_hostname(hostname), m_idleTimeout(idleTimeOut), 
								   m_TCPThreadActive(false), m_sendBuffer(NULL), 
								   m_sendBufferSize(0), m_TxBytes(0), m_RxBytes(0), 
								   m_maxClients(maxConnections)
{

}

TCPCommunication::~TCPCommunication()
{
	stopListening();
	if (m_sendBuffer != NULL) {
		delete [] m_sendBuffer;
		m_sendBuffer = NULL;
		m_sendBufferSize = 0;
	}
}

void TCPCommunication::startListening()
{
	if (m_TCPSocket.GetSocketCount() > 0) {
		_WARNING("Already listing on a TCP socket");
		return;
	}
	if (m_hostname == "0.0.0.0") {
		_MESSAGE("Start listening for TCP connections on:");
		IDebugLog::Indent();
		char name[250];
		PHOSTENT hostinfo;
		if (gethostname(name, sizeof(name)) == 0)
		{
			if ((hostinfo = gethostbyname(name)) != NULL)
			{
				int nCount = 0;
				while (hostinfo->h_addr_list[nCount])
				{
					char *ip = inet_ntoa(*(
						struct in_addr *)hostinfo->h_addr_list[nCount]);

					_MESSAGE("%s:%d", ip, m_port);
					nCount++;
				}
			}
		}
		IDebugLog::Outdent();
	}
	else {
		_MESSAGE("Start listening for TCP connections on: %s:%d", m_hostname.c_str(), m_port);
	}


	struct addrinfo hints;
	struct addrinfo *result;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	DWORD ip = 0;

	if (m_hostname != "0.0.0.0"){
		int iResult = getaddrinfo(m_hostname.c_str(), std::to_string(m_port).c_str(), &hints, &result);
		if (iResult != 0) {
			_WARNING("Error looking up hostname \"%s\": %d", m_hostname.c_str(), iResult);
		}
		else {
			struct sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *)result->ai_addr;
			_VMESSAGE("Resolved IP: %s", inet_ntoa(sockaddr_ipv4->sin_addr));
			ip = sockaddr_ipv4->sin_addr.s_addr;
			freeaddrinfo(result);
		}
	}


	DWORD err = m_TCPSocket.Listen(ip, m_port, 2000, m_idleTimeout);
	if (err != 0) {
		_ERROR("Could not start listening on TCP port");
		return;
	}

	// Spawning of new thread
	m_TCPThreadActive = true;
	m_TCPThread = std::thread(staticThreadHanlder, this);
}

void TCPCommunication::stopListening()
{
	m_TCPThreadActive = false;
	if (m_TCPSocket.GetSocketCount() > 0) {
		m_TCPSocket.Close();
	}
	if (m_TCPThread.joinable()) {
		m_TCPThread.join();
	}
}

bool TCPCommunication::isConnected()
{
	return (m_TCPSocket.GetState() & TCP::cSocket::E_Connected) != 0;
}

int TCPCommunication::getNumClients()
{
	if (isConnected()) {
		return m_TCPSocket.GetSocketCount() - 1;
	}
	return 0;
}

void TCPCommunication::getClients(std::vector<TCPClient> &clients)
{
	TCP::cSocket::cHash<SOCKET, DWORD> connectedSockets;
	DWORD err = m_TCPSocket.GetAllConnectedSockets(&connectedSockets);
	if (err != 0) {
		_ERROR("Could not get connected sockets: %d", err);
		return;
	}
	DWORD count = connectedSockets.GetCount();
	for (int i = 0; i < count; i++) {
		SOCKET socket = connectedSockets.GetKeyByIndex(i);
		clients.push_back(reinterpret_cast<TCPClient>(socket));
	}
}

std::string TCPCommunication::getClientIP(TCPClient &client)
{
	TCP::cSocket::cHash<SOCKET, DWORD> connectedSockets;
	DWORD err = m_TCPSocket.GetAllConnectedSockets(&connectedSockets);
	if (err != 0) {
		_ERROR("Could not get connected sockets: %d", err);
		return "0.0.0.0";
	}
	DWORD ip = connectedSockets.GetValueByKey(reinterpret_cast<SOCKET>(client));
	char tempBuf[3 * 4 + 4 + 1 + 3];
	_snprintf(tempBuf, sizeof(tempBuf), "%d.%d.%d.%d", (ip & 0xFF), ((ip >> 8) & 0xFF),
			 ((ip >> 16) & 0xFF), ((ip >> 24) & 0xFF));
	return tempBuf;
}

void TCPCommunication::disconnectClient(TCPClient client)
{
	SOCKET socket = reinterpret_cast<SOCKET>(client);
	DWORD err = m_TCPSocket.DisconnectClient(socket);
	if (err != 0) {
		_ERROR("Could not disconnect client %s: %d", getClientIP(client).c_str(), err);
	}
}

DataPacket * TCPCommunication::recvPacket(TCPClient *client)
{
	std::pair<TCPClient, DataPacket *> recvd;
	bool available;
	m_packetQueueMutex.lock();
	available = m_packetQueue.size() > 0;
	if (available) {
		recvd = m_packetQueue.front();
		m_packetQueue.pop();
	}
	m_packetQueueMutex.unlock();

	if (!available)
		return NULL;
	if (client != NULL)
		*client = recvd.first;
	return recvd.second;
}

void TCPCommunication::sendPacket(DataPacket *packet, TCPClient client)
{
	assert(isConnected());
	SOCKET socket = reinterpret_cast<SOCKET>(client);

	size_t len = packet->getSize();
	if (m_sendBuffer == NULL || m_sendBufferSize < len) {
		if (m_sendBuffer != NULL) {
			delete [] m_sendBuffer;
			m_sendBuffer = NULL;
		}
		m_sendBuffer = new char[len];
		m_sendBufferSize = len;
	}

	packet->fillBuffer(m_sendBuffer, m_sendBufferSize);

	m_TCPSocket.SendTo(socket, m_sendBuffer, len);
}

void TCPCommunication::sendPacket(DataPacket *packet)
{
	m_sendMutex.lock();
	assert(isConnected());

	size_t len = packet->getSize();
	if (m_sendBuffer == NULL || m_sendBufferSize < len) {
		if (m_sendBuffer != NULL) {
			delete [] m_sendBuffer;
			m_sendBuffer = NULL;
		}
		m_sendBuffer = new char[len];
		m_sendBufferSize = len;
	}
	packet->fillBuffer(m_sendBuffer, m_sendBufferSize);

	TCP::cSocket::cHash<SOCKET, DWORD> connectedSockets;
	DWORD err = m_TCPSocket.GetAllConnectedSockets(&connectedSockets);
	if (err != 0) {
		_ERROR("Could not get connected sockets: %d", err);
		m_sendMutex.unlock();
		return;
	}
	DWORD count = connectedSockets.GetCount();
	for (int i = 0; i < count; i++) {
		SOCKET socket = connectedSockets.GetKeyByIndex(i);
		DWORD result;
		do {
			result = m_TCPSocket.SendTo(socket, m_sendBuffer, len);
		} while (result == WSA_IO_PENDING);
	}
	m_sendMutex.unlock();
}

int TCPCommunication::getIdleTimeout()
{
	return m_idleTimeout;
}

void TCPCommunication::resetTxBytes()
{
	m_TxBytes = 0;
}

void TCPCommunication::resetRxBytes()
{
	m_RxBytes = 0;
}

size_t TCPCommunication::getTxBytes()
{
	return m_TxBytes;
}

size_t TCPCommunication::getRxBytes()
{
	return m_RxBytes;
}

void TCPCommunication::setMaxConnections(int max)
{
	m_maxClients = max;
}

int TCPCommunication::getMaxConnections()
{
	return m_maxClients;
}

DWORD TCPCommunication::staticThreadHanlder(TCPCommunication *thisptr)
{
	return thisptr->threadHandler();
}

DWORD TCPCommunication::threadHandler()
{
	DWORD err;
	DWORD event, ip, read, sent;
	SOCKET socket;
	TCP::cSocket::cMemory *recvMem;
	while (m_TCPThreadActive) {
		err = m_TCPSocket.ProcessEvents(&event, &ip, &socket, &recvMem, &read, &sent);

		if (err == ERROR_TIMEOUT)
			continue;

		if (event) {
			if ((event & FD_ACCEPT)) {
				if (getNumClients() > m_maxClients){
					m_TCPSocket.DisconnectClient(socket);
				}
			}
			if ((event & FD_READ) && recvMem != NULL){
				processReceivedData(recvMem, reinterpret_cast<TCPClient>(socket));
				m_RxBytes += read;
			}
			if ((event & FD_WRITE)) {
				m_TxBytes += sent;
			}
		}

		if (err != 0) {
			if (err == WSAENOTCONN) {
				// socket.close was called
				break;
			}
			
			_ERROR("Error in TCP event processing: %d", err);
			
			if (err != WSAECONNABORTED && // Other side was killed
				err != WSAECONNRESET && // Connection reset by peer
				err != WSAECONNREFUSED && // MAX num of clients connected
				err != WSAESHUTDOWN) // Sending data after shutdown
				break; // Abort the loop when these errors have not occurred
		}
	}
	closeSockets();

	return 0;
}

void TCPCommunication::closeSockets()
{
	if (m_TCPSocket.GetSocketCount()) {
		m_TCPSocket.Close();
		_MESSAGE("TCP socket closed");
	}
}

void TCPCommunication::processReceivedData(TCP::cSocket::cMemory *data, TCPClient client)
{
	while (1) {
		char *buffer = data->GetBuffer();
		DWORD len = data->GetLength();
		// Check if a complete packet is received
		if (!DataPacket::containsCompletePacket(buffer, len))
		{
			// Find valid header
			bool containsHeader = DataPacketHeader::containsHeader(len);
			bool validHeader = DataPacketHeader::isValidHeader(buffer, len);
			if ( containsHeader && !validHeader) {
				// There is enough data, but it isn't valid
				data->DeleteLeft(1);
				continue;
			} 

			return;
		}

		// Read the packet
		DataPacket *packet = GetDataPacketFromBuffer(buffer, len);

		// push the packet on the FIFO
		m_packetQueueMutex.lock();
		m_packetQueue.push(std::make_pair(client, packet));
		m_packetQueueMutex.unlock();

		// Remove packet from buffer
		DWORD packetSize = packet->getSize();
		data->DeleteLeft(packetSize);
	}
}

