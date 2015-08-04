#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include <string>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		2		int16		Protocol version
// 7		2		int16		Name length
// 9		len		char[]		Name

class HelloPacket :
	public DataPacket
{
public:
	HelloPacket(const char *buffer, size_t bufferSize);
	HelloPacket(short protocolVersion, std::string name);
	virtual ~HelloPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	short getProtocolVersion();
	std::string getName();

	virtual void fillBuffer(char *buffer, size_t bufferSize);
		
private:
	void readPacket(const char *buffer, size_t bufferSize);

	std::string m_name;
	short m_version;
};

