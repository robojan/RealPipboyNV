#include "DataPacket.h"
#include <cassert>

#include <cstdint>
#include <cstddef>
#include <cstring>

#include <winsock2.h>

DataPacketHeader::DataPacketHeader(const char *buffer, size_t bufferSize)
{
	assert(containsHeader(bufferSize));
	readHeader(buffer, bufferSize);
}

DataPacketHeader::DataPacketHeader(MessageType type, size_t size) :
	m_type(type), m_dataSize(size)
{

}

DataPacketHeader::~DataPacketHeader()
{

}

size_t DataPacketHeader::getDataSize()
{
	return m_dataSize;
}

MessageType DataPacketHeader::getType()
{
	return m_type;
}

size_t DataPacketHeader::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getHeaderSize());

	buffer[0] = 'R';
	buffer[1] = 'P';

	buffer[2] = (char) m_type;

	*((uint16_t *) (buffer + 3)) = htons((uint16_t)m_dataSize);
	return getHeaderSize();
}

size_t DataPacketHeader::getHeaderSize()
{
	return 2 + 1 + 2; // 1 byte for type and 4 for size + 2 for sync
}

bool DataPacketHeader::containsHeader(size_t bufferSize)
{
	return bufferSize >= getHeaderSize();
}

bool DataPacketHeader::isValidHeader(const char *buffer, size_t bufferSize)
{
	if (!containsHeader(bufferSize))
		return false;
	return (buffer[0] == 'R' && buffer[1] == 'P');
}

void DataPacketHeader::readHeader(const char *buffer, size_t bufferSize)
{
	assert(isValidHeader(buffer, bufferSize));
	m_type = (MessageType) buffer[2];
	m_dataSize = ntohs(*((uint16_t *)(buffer + 3)));
}

DataPacket::DataPacket(const char *buffer, size_t bufferSize) :
	m_header(DataPacketHeader(buffer, bufferSize)), m_data(NULL)
{
	int headerSize = DataPacketHeader::getHeaderSize();
	assert(m_header.getDataSize() + headerSize <= bufferSize);
	readPacket(buffer, bufferSize);
}

DataPacket::DataPacket(const DataPacketHeader &header) : 
	m_header(header), m_data(NULL)
{

}

DataPacket::DataPacket() :
	m_header(MessageType::UNK, 0), m_data(NULL)
{

}

void DataPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	size_t dataSize = m_header.getDataSize();
	m_data = new unsigned char[dataSize];
	memcpy(m_data, buffer + DataPacketHeader::getHeaderSize(), dataSize);
}

DataPacket::~DataPacket()
{
	if (m_data != NULL) {
		delete [] m_data;
		m_data = NULL;
	}
}

MessageType DataPacket::getType()
{
	return m_header.getType();
}

size_t DataPacket::getSize()
{
	return m_header.getHeaderSize() + m_header.getDataSize();
}

void DataPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	memcpy(buffer + offset, m_data, m_header.getDataSize());
}

bool DataPacket::containsCompletePacket(const char *buffer, size_t bufferSize)
{
	if (!DataPacketHeader::isValidHeader(buffer, bufferSize))
		return false;
	DataPacketHeader header(buffer, bufferSize);
	return header.getHeaderSize() + header.getDataSize() <= bufferSize;
}

bool DataPacket::isValid() {
	return m_valid;
}
