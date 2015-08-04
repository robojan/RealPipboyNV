#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include <string>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		2		int16		fileName length
// 7		n		char[]		fileName
// ..		1		int8		id
// ..		4		uint32		fileLength
// ..		4		int32		number of packets

class SetFilePacket :
	public DataPacket
{
public:
	SetFilePacket(const char *buffer, size_t bufferSize);
	SetFilePacket(const std::string name, int8_t id, uint32_t fileLength, int32_t numberOfPackets);
	SetFilePacket(const std::string name, int8_t id);
	virtual ~SetFilePacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

private:
	void readPacket(const char *buffer, size_t bufferSize);

	std::string m_fileName;
	uint32_t m_fileLength;
	int32_t m_numPackets;
	int8_t  m_id;
};

