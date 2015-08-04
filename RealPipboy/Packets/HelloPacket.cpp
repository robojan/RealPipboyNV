#include "HelloPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

HelloPacket::HelloPacket(const char *buffer, size_t bufferSize) :
	DataPacket(DataPacketHeader(buffer, bufferSize)), m_version(0)
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

HelloPacket::HelloPacket(short protocolVersion, std::string name) :
m_name(name), m_version(protocolVersion)
{
	m_header = DataPacketHeader(getType(), getSize()-DataPacketHeader::getHeaderSize());
	m_valid = true;
}

HelloPacket::~HelloPacket()
{

}

MessageType HelloPacket::getType()
{
	return MessageType::HELLO;
}

size_t HelloPacket::getSize()
{
	return DataPacketHeader::getHeaderSize() + 4 + m_name.length();
}

short HelloPacket::getProtocolVersion()
{
	return m_version;
}

std::string HelloPacket::getName()
{
	return m_name;
}

void HelloPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	assert(((int)m_name.length()) <= INT16_MAX);
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	*((int16_t *) (buffer + 0)) = htons(m_version);
	*((int16_t *) (buffer + 2)) = htons((uint16_t)m_name.length());
	memcpy(buffer + 4, m_name.c_str(), m_name.length());
}

void HelloPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();
	m_version = ntohs(*((int16_t *) (buffer + 0)));
	short len = ntohs(*((int16_t *) (buffer + 2)));
	if (len > 0 && len <= ((int)bufferSize) - 4) {
		m_valid = true;
		m_name.append(buffer + 4, len);
	}
	else {
		m_valid = false;
	}

}
