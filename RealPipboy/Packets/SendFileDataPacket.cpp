#include "SendFileDataPacket.h"
#include <cassert>
#include <cstdint>
#include <winsock2.h>

SendFileDataPacket::SendFileDataPacket(const char *buffer, size_t bufferSize) :
	DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SendFileDataPacket::SendFileDataPacket(int8_t id, uint32_t offset, int32_t chunkIndex,
	const uint8_t *data, uint16_t dataLen) :
	m_id(id), m_offset(offset), m_chunkIndex(chunkIndex), m_data(data), m_dataLen(dataLen)
{
	assert(getSize() <= 64 * 1024);

	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SendFileDataPacket::~SendFileDataPacket()
{

}

MessageType SendFileDataPacket::getType()
{
	return MessageType::SENDFILEDATA;
}

size_t SendFileDataPacket::getSize()
{
	return DataPacketHeader::getHeaderSize() + 9 + m_dataLen;
}

void SendFileDataPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;

	buffer[0] = m_id;
	*((uint32_t *)(buffer + 1)) = htonl(m_offset);
	*((uint32_t *)(buffer + 5)) = htonl(m_chunkIndex);
	memcpy(buffer + 9, m_data, m_dataLen);
}

void SendFileDataPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	m_valid = true;
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();

	m_id = buffer[0];
	m_offset = ntohl(*((uint32_t *)(buffer + 1)));
	m_chunkIndex = ntohl(*((uint32_t *)(buffer + 5)));
	m_dataLen = (uint16_t)(m_header.getDataSize() - 9);
	uint8_t *data = new uint8_t[m_dataLen];
	memcpy(data, buffer + 9, m_dataLen);
	m_data = data;
}
