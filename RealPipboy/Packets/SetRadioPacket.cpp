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

SetRadioPacket::SetRadioPacket()
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
	return DataPacketHeader::getHeaderSize();
}

void SetRadioPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
}

void SetRadioPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	m_valid = true;
}
