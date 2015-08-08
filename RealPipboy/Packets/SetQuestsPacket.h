#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include "../DataTypes/Quest.h"

#include <string>
#include <vector>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		2		int16		Number of quests
// each quest:
// 0		4		int32		questID
// 4		4		int32		flags
// 8		2		int16		nameLen
// 10		n		char[]		name
// ..		2		int16		number of objectives
// each objective
// 0		4		int32		objectiveID
// 4		4		int32		flags
// 8		2		int16		textLen
// 10		n		char[]		text


class SetQuestsPacket :
	public DataPacket
{
public:
	SetQuestsPacket(const char *buffer, size_t bufferSize);
	SetQuestsPacket(const std::vector<Quest *> &list, bool deleteQuests);
	virtual ~SetQuestsPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

private:
	void readPacket(const char *buffer, size_t bufferSize);

	std::vector<Quest *> m_quests;
	bool m_deleteQuests;
};

