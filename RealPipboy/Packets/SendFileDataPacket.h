#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include <string>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		1		int8		file id
// 6		4		uint32		offset
// 10		4		int32		chunk index
// 14		dataLen uint8[]		data

class SendFileDataPacket :
	public DataPacket
{
public:
	SendFileDataPacket(const char *buffer, size_t bufferSize);
	SendFileDataPacket(int8_t id, uint32_t offset, int32_t chunkIndex, const uint8_t *data, uint16_t dataLen);
	virtual ~SendFileDataPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

private:
	void readPacket(const char *buffer, size_t bufferSize);

	int8_t  m_id;
	uint32_t m_offset;
	int32_t m_chunkIndex;
	const uint8_t *m_data;
	uint16_t m_dataLen;
};

