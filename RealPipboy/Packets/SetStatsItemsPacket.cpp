#include "SetStatsItemsPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetStatsItemsPacket::SetStatsItemsPacket(const char *buffer, size_t bufferSize) :
DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}
SetStatsItemsPacket::SetStatsItemsPacket(signed char statsType, const std::vector<StatisticsInfoItem> &stats) :
m_statsType(statsType), m_stats(stats)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetStatsItemsPacket::~SetStatsItemsPacket()
{

}

MessageType SetStatsItemsPacket::getType()
{
	return MessageType::SETSTATSITEMS;
}

size_t SetStatsItemsPacket::getSize()
{
	int statsSize = 0;
	for (std::vector<StatisticsInfoItem>::iterator it = m_stats.begin(); it != m_stats.end();
		 ++it) {
		statsSize += it->getBadge().length() + 2;
		statsSize += it->getIcon().length() + 2;
		statsSize += it->getName().length() + 2;
		statsSize += it->getDescription().length() + 2;
		statsSize += it->getValue().length() + 2;
		statsSize += it->getExtra().length() + 2;
	}
	return DataPacketHeader::getHeaderSize() + 1 + statsSize + m_stats.size() * 2;
}

void SetStatsItemsPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	buffer[0] = m_statsType;
	buffer++;
	for (std::vector<StatisticsInfoItem>::iterator it = m_stats.begin(); it != m_stats.end();
		 ++it) {
		// Calculate the size of this stat
		int statSize = it->getBadge().length() + 2;
		statSize += it->getIcon().length() + 2;
		statSize += it->getName().length() + 2;
		statSize += it->getDescription().length() + 2;
		statSize += it->getValue().length() + 2;
		statSize += it->getExtra().length() + 2;
		// write stat size
		*((int16_t *) (buffer + 0)) = htons(statSize);
		buffer += 2;
		// Write stat
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getName().length());
		memcpy(buffer + 2, it->getName().c_str(), it->getName().length());
		buffer += 2 + it->getName().length();
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getValue().length());
		memcpy(buffer + 2, it->getValue().c_str(), it->getValue().length());
		buffer += 2 + it->getValue().length();
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getExtra().length());
		memcpy(buffer + 2, it->getExtra().c_str(), it->getExtra().length());
		buffer += 2 + it->getExtra().length();
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getDescription().length());
		memcpy(buffer + 2, it->getDescription().c_str(), it->getDescription().length());
		buffer += 2 + it->getDescription().length();
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getIcon().length());
		memcpy(buffer + 2, it->getIcon().c_str(), it->getIcon().length());
		buffer += 2 + it->getIcon().length();
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getBadge().length());
		memcpy(buffer + 2, it->getBadge().c_str(), it->getBadge().length());
		buffer += 2 + it->getBadge().length();		
	}
}

const std::vector<StatisticsInfoItem> & SetStatsItemsPacket::getStats() const
{
	return m_stats;
}

signed char SetStatsItemsPacket::getStatsType() const
{
	return m_statsType;
}

void SetStatsItemsPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();

	m_statsType = buffer[0];
	buffer++;
	bufferSize--;
	while (bufferSize > 2) {
		const char *statStart = buffer;
		short statSize = ntohs(*(int16_t *) (buffer + 0));
		// name
		short nameLen = ntohs(*(int16_t *) (buffer + 2));
		std::string name(buffer + 4, nameLen);
		buffer += 4 + nameLen;
		bufferSize -= 4 + nameLen;
		// Value
		short valueLen = ntohs(*(int16_t *) (buffer + 0));
		std::string value(buffer + 2, valueLen);
		buffer += 2 + valueLen;
		bufferSize -= 2 + valueLen;
		// Extra
		short extraLen = ntohs(*(int16_t *) (buffer + 0));
		std::string extra(buffer + 2, extraLen);
		buffer += 2 + extraLen;
		buffer -= 2 + extraLen;
		// Description
		short descriptionLen = ntohs(*(int16_t *) (buffer + 0));
		std::string description(buffer + 2, descriptionLen);
		buffer += 2 + descriptionLen;
		buffer -= 2 + descriptionLen;
		// Icon
		short iconLen = ntohs(*(int16_t *) (buffer + 0));
		std::string icon(buffer + 2, iconLen);
		buffer += 2 + iconLen;
		buffer -= 2 + iconLen;
		// Badge
		short badgeLen = ntohs(*(int16_t *) (buffer + 0));
		std::string badge(buffer + 2, badgeLen);
		buffer += 2 + badgeLen;
		buffer -= 2 + badgeLen;

		m_stats.push_back(StatisticsInfoItem(name, icon, badge, description, value, extra));

		buffer = statStart + statSize+2;
	}
	m_valid = true;
}
