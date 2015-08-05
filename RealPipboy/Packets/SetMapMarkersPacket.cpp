#include "SetMapMarkersPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetMapMarkersPacket::SetMapMarkersPacket(const char *buffer, size_t bufferSize) :
	DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SetMapMarkersPacket::SetMapMarkersPacket(const std::vector<MapMarker *> &markers) :
	m_markers(markers)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetMapMarkersPacket::~SetMapMarkersPacket()
{
	for (std::vector<MapMarker *>::iterator it = m_markers.begin(); it != m_markers.end(); ++it) {
		delete *it;
	}
}

MessageType SetMapMarkersPacket::getType()
{
	return MessageType::SETMAPMARKERS;
}

size_t SetMapMarkersPacket::getSize()
{
	int dataSize = 2;
	for (std::vector<MapMarker *>::iterator it = m_markers.begin(); it != m_markers.end(); ++it) {
		dataSize += 20 + (*it)->getName().length() + (*it)->getRepuation().length();
	}

	return DataPacketHeader::getHeaderSize() + dataSize;
}

void SetMapMarkersPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	*((int16_t *)(buffer + 0)) = htons((uint16_t)m_markers.size());
	buffer += 2;
	bufferSize -= 2;
	for (std::vector<MapMarker *>::iterator it = m_markers.begin(); it != m_markers.end(); ++it) {
		*((int16_t *)(buffer + 0)) = htons((*it)->getType());
		*((int16_t *)(buffer + 2)) = htons((*it)->getFlags());
		*((uint32_t *)(buffer + 4)) = htonf((*it)->getX());
		*((uint32_t *)(buffer + 8)) = htonf((*it)->getY());
		*((uint32_t *)(buffer + 12)) = htonf((*it)->getZ());
		*((int16_t *)(buffer + 16)) = htons((uint16_t)(*it)->getName().length());
		memcpy(buffer + 18, (*it)->getName().c_str(), (*it)->getName().length());
		buffer += 18 + (*it)->getName().length();
		bufferSize -= 18 + (*it)->getName().length();
		*((int16_t *)(buffer + 0)) = htons((uint16_t)(*it)->getRepuation().length());
		memcpy(buffer + 2, (*it)->getRepuation().c_str(), (*it)->getRepuation().length());
		buffer += 2 + (*it)->getRepuation().length();
		bufferSize -= 2 + (*it)->getRepuation().length();
	}
}

void SetMapMarkersPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();

	m_markers.clear();

	int numMarkers = ntohs(*((int16_t *)(buffer + 0)));
	buffer += 2;
	bufferSize -= 2;
	for (int i = 0; i < numMarkers && bufferSize >= 6; i++) {
		enum MapMarker::Type type = (enum MapMarker::Type)ntohs(*((int16_t *)(buffer + 0)));
		uint16_t flags = ntohs(*((int16_t *)(buffer + 2)));
		float x = ntohf(*((uint32_t *)(buffer + 4)));
		float y = ntohf(*((uint32_t *)(buffer + 8)));
		float z = ntohf(*((uint32_t *)(buffer + 12)));
		int nameLen = ntohs(*((int16_t *)(buffer + 16)));
		std::string name(buffer + 18, nameLen);
		int reputationLen = ntohs(*((int16_t *)(buffer + 18 + nameLen)));
		std::string reputation(buffer + 20 + nameLen, reputationLen);
		m_markers.push_back(new MapMarker(x, y, z, type, name, flags, reputation));
		buffer += 20 + nameLen + reputationLen;
		bufferSize -= 20 + nameLen + reputationLen;
	}
	m_valid = true;
}
