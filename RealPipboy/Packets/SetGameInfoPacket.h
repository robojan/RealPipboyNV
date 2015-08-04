#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include <string>
#include <cstdint>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		1		int8		Game Version
// 6		1		bool		hardcore mode

class SetGameInfoPacket :
	public DataPacket
{
public:
	SetGameInfoPacket(const char *buffer, size_t bufferSize);
	SetGameInfoPacket(int8_t gameVersion, bool hardcore);
	virtual ~SetGameInfoPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	int8_t getGameVersion();
	bool getHardcore();

	virtual void fillBuffer(char *buffer, size_t bufferSize);
		
private:
	void readPacket(const char *buffer, size_t bufferSize);

	int8_t m_gameVersion;
	bool m_hardcore;
};

