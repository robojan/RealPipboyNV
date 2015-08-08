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

SetMapMarkersPacket::SetMapMarkersPacket(const std::vector<MapMarker *> &markers, bool deleteMarkers) :
	m_markers(markers), m_deleteMarkers(deleteMarkers)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetMapMarkersPacket::~SetMapMarkersPacket()
{
	if (m_deleteMarkers) {
		for (std::vector<MapMarker *>::iterator it = m_markers.begin(); it != m_markers.end(); ++it) {
			delete *it;
		}
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
		dataSize += 24 + (*it)->getName().length() + (*it)->getRepuation().length();
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
		*((uint32_t *)(buffer + 4)) = htonl((*it)->getID());
		*((uint32_t *)(buffer + 8)) = htonf((*it)->getX());
		*((uint32_t *)(buffer + 12)) = htonf((*it)->getY());
		*((uint32_t *)(buffer + 16)) = htonf((*it)->getZ());
		*((int16_t *)(buffer + 20)) = htons((uint16_t)(*it)->getName().length());
		memcpy(buffer + 22, (*it)->getName().c_str(), (*it)->getName().length());
		buffer += 22 + (*it)->getName().length();
		bufferSize -= 22 + (*it)->getName().length();
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
		uint32_t id = ntohl(*((uint32_t *)(buffer + 4)));
		float x = ntohf(*((uint32_t *)(buffer + 8)));
		float y = ntohf(*((uint32_t *)(buffer + 12)));
		float z = ntohf(*((uint32_t *)(buffer + 16)));
		int nameLen = ntohs(*((int16_t *)(buffer + 20)));
		std::string name(buffer + 22, nameLen);
		int reputationLen = ntohs(*((int16_t *)(buffer + 22 + nameLen)));
		std::string reputation(buffer + 24 + nameLen, reputationLen);
		m_markers.push_back(new MapMarker(id, x, y, z, type, name, flags, reputation));
		buffer += 24 + nameLen + reputationLen;
		bufferSize -= 24 + nameLen + reputationLen;
	}
	m_valid = true;
}
