#include "SetWorldInfoPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetWorldInfoPacket::SetWorldInfoPacket(const char *buffer, size_t bufferSize) :
DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SetWorldInfoPacket::SetWorldInfoPacket(short year, signed char month, signed char day,
	signed char hour, signed char minute, const std::string &location,
	int usableMapWidth, int usableMapHeight, int16_t cellNWX, int16_t cellNWY,
	int16_t cellSEX, int16_t cellSEY, float mapScale, float mapOffsetX, float mapOffsetY,
	const std::string &mapPath) :
	m_year(year), m_month(month), m_day(day), m_hour(hour), m_minute(minute), 
	m_locationName(location), m_usableWidth(usableMapWidth), m_usableHeight(usableMapHeight),
	m_cellNWX(cellNWX), m_cellNWY(cellNWY), m_cellSEX(cellSEX), m_cellSEY(cellSEY),
	m_mapScale(mapScale), m_mapOffsetX(mapOffsetX), m_mapOffsetY(mapOffsetY), m_mapPath(mapPath)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetWorldInfoPacket::~SetWorldInfoPacket()
{

}

MessageType SetWorldInfoPacket::getType()
{
	return MessageType::SETWORLDINFO;
}

size_t SetWorldInfoPacket::getSize()
{
	return DataPacketHeader::getHeaderSize() + 8 + m_locationName.length() +
		30 + m_mapPath.length();
}

void SetWorldInfoPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	*((int16_t *) (buffer + 0)) = htons(m_year);
	buffer[2] = m_month;
	buffer[3] = m_day;
	buffer[4] = m_hour;
	buffer[5] = m_minute;
	*((int16_t *) (buffer + 6)) = htons((uint16_t)m_locationName.length());
	memcpy(buffer + 8, m_locationName.c_str(), m_locationName.length());
	buffer += 8 + m_locationName.length();
	bufferSize -= 8 + m_locationName.length();
	*((uint32_t *)(buffer + 0)) = htonl(m_usableWidth);
	*((uint32_t *)(buffer + 4)) = htonl(m_usableHeight);
	*((uint16_t *)(buffer + 8)) = htons(m_cellNWX);
	*((uint16_t *)(buffer + 10)) = htons(m_cellNWY);
	*((uint16_t *)(buffer + 12)) = htons(m_cellSEX);
	*((uint16_t *)(buffer + 14)) = htons(m_cellSEY);
	*((uint32_t *)(buffer + 16)) = htonf(m_mapScale);
	*((uint32_t *)(buffer + 20)) = htonf(m_mapOffsetX);
	*((uint32_t *)(buffer + 24)) = htonf(m_mapOffsetY);
	*((uint16_t *)(buffer + 28)) = htons((short)m_mapPath.length());
	memcpy(buffer + 30, m_mapPath.c_str(), m_mapPath.length());
}

void SetWorldInfoPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();
	m_year = ntohs(*(int16_t *) (buffer + 0));
	m_month = buffer[2];
	m_day = buffer[3];
	m_hour = buffer[4];
	m_minute = buffer[5];
	short nameLen = ntohs(*(int16_t *)(buffer + 6));
	m_locationName = std::string(buffer + 8, nameLen);
	buffer += 8 + nameLen;
	bufferSize -= 8 + nameLen;
	m_usableWidth = ntohl(*(uint32_t *)(buffer + 0));
	m_usableHeight = ntohl(*(uint32_t *)(buffer + 4));
	m_cellNWX = ntohs(*(uint16_t *)(buffer + 8));
	m_cellNWY = ntohs(*(uint16_t *)(buffer + 10));
	m_cellSEX = ntohs(*(uint16_t *)(buffer + 12));
	m_cellSEY = ntohs(*(uint16_t *)(buffer + 14));
	m_mapScale = ntohf(*(uint32_t *)(buffer + 16));
	m_mapOffsetX = ntohf(*(uint32_t *)(buffer + 20));
	m_mapOffsetY = ntohf(*(uint32_t *)(buffer + 24));
	short pathLen = ntohs(*(uint16_t *)(buffer + 28));
	m_mapPath = std::string(buffer + 30, pathLen);
	m_valid = true;
}


short SetWorldInfoPacket::getYear() const
{
	return m_year;
}

signed char SetWorldInfoPacket::getMonth() const
{
	return m_month;
}

signed char SetWorldInfoPacket::getDay() const
{
	return m_day;
}

signed char SetWorldInfoPacket::getHour() const
{
	return m_hour;
}

signed char SetWorldInfoPacket::getMinute() const
{
	return m_minute;
}

const std::string & SetWorldInfoPacket::getLocation() const
{
	return m_locationName;
}

void SetWorldInfoPacket::setYear(short year)
{
	m_year = year;
}

void SetWorldInfoPacket::setMonth(signed char month)
{
	m_month = month;
}

void SetWorldInfoPacket::setDay(signed char day)
{
	m_day = day;
}

void SetWorldInfoPacket::setHour(signed char hour)
{
	m_hour = hour;
}

void SetWorldInfoPacket::setMinute(signed char minute)
{
	m_minute = minute;
}