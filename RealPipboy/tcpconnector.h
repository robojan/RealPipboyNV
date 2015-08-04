#pragma once

class TCPStream;

class TCPConnector
{
public:
	TCPConnector();
	~TCPConnector();
	TCPStream * connect(int port);
};

