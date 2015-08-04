#include "GetFilePacket.h"

#include "common/IPrefix.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

GetFilePacket::GetFilePacket(const char *buffer, size_t bufferSize) :
	DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

GetFilePacket::GetFilePacket(int8_t id, int8_t flags, const std::string &fileName) :
	m_fileName(fileName), m_id(id), m_flags(flags)
{
	m_header = DataPacketHeader(getType(), getSize()-DataPacketHeader::getHeaderSize());
	m_valid = true;
}

GetFilePacket::~GetFilePacket()
{

}

MessageType GetFilePacket::getType()
{
	return MessageType::GETFILE;
}

size_t GetFilePacket::getSize()
{
	return DataPacketHeader::getHeaderSize() + 4 + m_fileName.length();
}

const std::string & GetFilePacket::getFileName() const
{
	return m_fileName;
}

int8_t GetFilePacket::getID() const
{
	return m_id;
}

int8_t GetFilePacket::getFlags() const
{
	return m_flags;
}

void GetFilePacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	buffer[0] = m_id;
	buffer[1] = m_flags;
	*((int16_t *)(buffer + 2)) = htons((short)m_fileName.length());
	memcpy(buffer + 4, m_fileName.c_str(), m_fileName.length());
}

void GetFilePacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();
	m_id = buffer[0];
	m_flags = buffer[1];
	short nameLen = ntohs(*((int16_t *)(buffer + 2)));
	m_fileName = std::string(buffer + 4, nameLen);
	m_valid = true;
}
