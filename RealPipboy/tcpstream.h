#ifndef TCPSTREAM_H_
#define TCPSTREAM_H_

#include <WinSock2.h>

class TCPStream
{
public:
	TCPStream(SOCKET socket);
	TCPStream(TCPStream &stream);
	~TCPStream();

	size_t write(const char * buffer, size_t size);
	size_t read(char *buffer, size_t size);

private:
	SOCKET m_socket;
};

#endif TCPSTREAM_H_