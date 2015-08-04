#include "tcpstream.h"


TCPStream::TCPStream(SOCKET socket) :
m_socket(socket)
{
}


TCPStream::TCPStream(TCPStream &stream) :
m_socket(stream.m_socket)
{
	
}

TCPStream::~TCPStream()
{
	closesocket(m_socket);
}


size_t TCPStream::write(const char * buffer, size_t size)
{
	return 0;
}


size_t TCPStream::read(char *buffer, size_t size)
{
	return 0;
}
