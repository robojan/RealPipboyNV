#include "DoActionPacket.h"

#include "common/IPrefix.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

DoActionPacket::DoActionPacket(const char *buffer, size_t bufferSize) :
	DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

DoActionPacket::DoActionPacket(int32_t id, uint32_t param1, uint32_t param2, uint32_t param3, 
	uint32_t param4) :
	m_id(id)
{
	m_param[0] = param1;
	m_param[1] = param2;
	m_param[2] = param3;
	m_param[3] = param4;

	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

DoActionPacket::~DoActionPacket()
{

}

MessageType DoActionPacket::getType()
{
	return MessageType::DOACTION;
}

size_t DoActionPacket::getSize()
{
	return DataPacketHeader::getHeaderSize() + 20;
}

int32_t DoActionPacket::getID() const
{
	return m_id;
}

uint32_t DoActionPacket::getParam(int index) const
{
	return m_param[index];
}

void DoActionPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	*((int32_t *)(buffer + 0)) = htonl(m_id);
	*((int32_t *)(buffer + 4)) = htons(m_param[0]);
	*((int32_t *)(buffer + 8)) = htons(m_param[1]);
	*((int32_t *)(buffer + 12)) = htons(m_param[2]);
	*((int32_t *)(buffer + 16)) = htons(m_param[3]);
}

void DoActionPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();
	m_id = ntohl(*((uint32_t *)(buffer + 0)));
	m_param[0] = ntohl(*((uint32_t *)(buffer + 4)));
	m_param[1] = ntohl(*((uint32_t *)(buffer + 8)));
	m_param[2] = ntohl(*((uint32_t *)(buffer + 12)));
	m_param[3] = ntohl(*((uint32_t *)(buffer + 16)));
	m_valid = true;
}
