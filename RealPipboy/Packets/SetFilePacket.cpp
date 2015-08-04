#include "SetFilePacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetFilePacket::SetFilePacket(const char *buffer, size_t bufferSize) :
DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SetFilePacket::SetFilePacket(const std::string name, int8_t id, uint32_t fileLength, 
	int32_t numberOfPackets) :
	m_fileName(name), m_id(id), m_fileLength(fileLength), m_numPackets(numberOfPackets)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetFilePacket::SetFilePacket(const std::string name, int8_t id) :
	m_fileName(name), m_id(id), m_fileLength(0), m_numPackets(-1)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetFilePacket::~SetFilePacket()
{

}

MessageType SetFilePacket::getType()
{
	return MessageType::SETFILE;
}

size_t SetFilePacket::getSize()
{
	return DataPacketHeader::getHeaderSize() + 11 + m_fileName.length();
}

void SetFilePacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	
	*((int16_t *)(buffer + 0)) = htons((short)m_fileName.length());
	memcpy(buffer + 2, m_fileName.c_str(), m_fileName.length());
	buffer += 2 + m_fileName.length();
	bufferSize -= 2 + m_fileName.length();
	buffer[0] = m_id;
	*((uint32_t *)(buffer + 1)) = htonl(m_fileLength);
	*((uint32_t *)(buffer + 5)) = htonl(m_numPackets);
	
}

void SetFilePacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	m_valid = true;
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();

	int nameLen = ntohs(*((uint16_t *)(buffer + 0)));
	m_fileName = std::string(buffer + 2, nameLen);
	buffer += 2 + nameLen;
	bufferSize -= 2 + nameLen;
	m_id = buffer[0];
	m_fileLength = ntohl(*((uint32_t *)(buffer + 1)));
	m_numPackets = ntohl(*((uint32_t *)(buffer + 5)));
}
