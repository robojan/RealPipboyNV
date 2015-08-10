#include "SetRadioPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetRadioPacket::SetRadioPacket(const char *buffer, size_t bufferSize) :
DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SetRadioPacket::SetRadioPacket(const std::vector<Radio> &stations) :
	m_stations(stations)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetRadioPacket::~SetRadioPacket()
{

}

MessageType SetRadioPacket::getType()
{
	return MessageType::SETRADIO;
}

size_t SetRadioPacket::getSize()
{
	size_t dataSize = 0;
	for (std::vector<Radio>::iterator it = m_stations.begin(); it != m_stations.end(); ++it) {
		dataSize += 7 + it->getName().length();
	}
	return DataPacketHeader::getHeaderSize() + 2 + dataSize;
}

void SetRadioPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	*((uint16_t *)(buffer + 0)) = htons((uint16_t)m_stations.size());
	buffer += 2;
	bufferSize -= 2;
	for (std::vector<Radio>::iterator it = m_stations.begin(); it != m_stations.end(); ++it) {
		*((uint32_t *)(buffer + 0)) = htonl(it->getId());
		buffer[4] = it->getFlags();
		*((uint16_t *)(buffer + 5)) = htons((short)it->getName().length());
		memcpy(buffer + 7, it->getName().c_str(), it->getName().length());
		buffer += 7 + it->getName().length();
		bufferSize -= 7 + it->getName().length();
	}

}

void SetRadioPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();

	m_stations.clear();

	int numStations = ntohs(*((int16_t *)(buffer + 0)));
	buffer += 2;
	bufferSize -= 2;
	for (int i = 0; i < numStations && bufferSize >= 7; i++) {
		int id = ntohl(*((int32_t *)(buffer + 0)));
		unsigned char flags = buffer[4];
		int nameLen = ntohs(*((int16_t *)(buffer + 5)));
		std::string name(buffer + 7, nameLen);
		m_stations.push_back(Radio(id, name, flags));
		buffer += 7 + nameLen;
		bufferSize -= 7 + nameLen;
	}
	m_valid = true;
}
