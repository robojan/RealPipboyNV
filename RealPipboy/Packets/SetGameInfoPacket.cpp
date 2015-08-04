#include "SetGameInfoPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetGameInfoPacket::SetGameInfoPacket(const char *buffer, size_t bufferSize) :
	DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SetGameInfoPacket::SetGameInfoPacket(int8_t gameVersion, bool hardcore) :
m_gameVersion(gameVersion), m_hardcore(hardcore)
{
	m_header = DataPacketHeader(getType(), getSize()-DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetGameInfoPacket::~SetGameInfoPacket()
{

}

MessageType SetGameInfoPacket::getType()
{
	return MessageType::SETGAMEINFO;
}

size_t SetGameInfoPacket::getSize()
{
	return DataPacketHeader::getHeaderSize() + 2;
}

int8_t SetGameInfoPacket::getGameVersion()
{
	return m_gameVersion;
}

bool SetGameInfoPacket::getHardcore()
{
	return m_hardcore;
}

void SetGameInfoPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= bufferSize;
	buffer[0] = m_gameVersion;
	buffer[1] = m_hardcore ? 1 : 0;
}

void SetGameInfoPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();
	m_gameVersion = buffer[0];
	m_hardcore = buffer[1] != 0;
	m_valid = true;
}
