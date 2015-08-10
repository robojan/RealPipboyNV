#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include "../DataTypes/Radio.h"
#include <string>
#include <vector>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		2		int16		num radio stations
// each radio 
// 0		4		int32		id
// 4		1		int8		flags
// 5		2		int16		nameLen
// 7		n		char[]		name

class SetRadioPacket :
	public DataPacket
{
public:
	SetRadioPacket(const char *buffer, size_t bufferSize);
	SetRadioPacket(const std::vector<Radio> &stations);
	virtual ~SetRadioPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	std::vector<Radio> &getRadioStations();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

private:
	void readPacket(const char *buffer, size_t bufferSize);

	std::vector<Radio> m_stations;
};

