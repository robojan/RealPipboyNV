#pragma once

#include "../DataTypes/StatisticsInfoItem.h"
#include "DataPacket.h"
#include "MessageTypes.h"
#include <vector>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 4		1		int8		Type of stats
// 5		2		int16		Size of stat
// 7		n		stat		statInfo

// Statinfo
// 0		2		int16		len Name
// 2		n		char[]		name
// ..		2		int16		value len
// ..		n		char[]		value
// ..		2		int16		extra len
// ..		n		char[]		extra
// ..		2		int16		description len
// ..		n		char[]		description
// ..		2		int16		icon len
// ..		n		char[]		icon
// ..		2		int16		badge len
// ..		n		char[]		badge

// type of stats:
// 0 Special
// 1 Skills
// 2 Perks
// 3 Statistics

class SetStatsItemsPacket :
	public DataPacket
{
public:
	SetStatsItemsPacket(const char *buffer, size_t bufferSize);
	SetStatsItemsPacket(signed char statsType, const std::vector<StatisticsInfoItem> &stats);
	virtual ~SetStatsItemsPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

	const std::vector<StatisticsInfoItem> &getStats() const;
	signed char getStatsType() const;

private:
	void readPacket(const char *buffer, size_t bufferSize);

	std::vector<StatisticsInfoItem> m_stats;
	signed char m_statsType;
};

