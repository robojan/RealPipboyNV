#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include <string>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet

class SetRadioPacket :
	public DataPacket
{
public:
	SetRadioPacket(const char *buffer, size_t bufferSize);
	SetRadioPacket();
	virtual ~SetRadioPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

private:
	void readPacket(const char *buffer, size_t bufferSize);
};

