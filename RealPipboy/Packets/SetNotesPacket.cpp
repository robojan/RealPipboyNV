#include "SetNotesPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetNotesPacket::SetNotesPacket(const char *buffer, size_t bufferSize) :
	DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SetNotesPacket::SetNotesPacket(const std::vector<Note *> &notes, bool deleteNotes) :
m_notes(notes), m_deleteNotes(deleteNotes)
{
	m_header = DataPacketHeader(getType(), getSize()-DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetNotesPacket::~SetNotesPacket()
{
	if (m_deleteNotes) {
		for (std::vector<Note *>::iterator it = m_notes.begin(); it != m_notes.end(); ++it) {
			delete *it;
		}
	}
}

MessageType SetNotesPacket::getType()
{
	return MessageType::SETNOTES;
}

size_t SetNotesPacket::getSize()
{
	int dataSize = 2;
	for (std::vector<Note *>::iterator it = m_notes.begin(); it != m_notes.end(); ++it) {
		dataSize += 6 + (*it)->getTitle().length() + (*it)->getContent().length();
	}

	return DataPacketHeader::getHeaderSize() + dataSize;
}

void SetNotesPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	*((int16_t *)(buffer + 0)) = htons((uint16_t)m_notes.size());
	buffer += 2;
	bufferSize -= 2;
	for (std::vector<Note *>::iterator it = m_notes.begin(); it != m_notes.end(); ++it) {
		buffer[0] = (*it)->getType();
		buffer[1] = (*it)->getFlags();
		*((int16_t *) (buffer + 2)) = htons((uint16_t)(*it)->getTitle().length());
		memcpy(buffer + 4, (*it)->getTitle().c_str(), (*it)->getTitle().length());
		buffer += 4 + (*it)->getTitle().length();
		bufferSize -= 4 + (*it)->getTitle().length();
		*((int16_t *) (buffer + 0)) = htons((uint16_t)(*it)->getContent().length());
		memcpy(buffer + 2, (*it)->getContent().c_str(), (*it)->getContent().length());
		buffer += 2 + (*it)->getContent().length();
		bufferSize -= 2 + (*it)->getContent().length();
	}
}

void SetNotesPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();

	int numNotes = ntohs(*((int16_t *)(buffer + 0)));
	buffer += 2;
	bufferSize -= 2;
	for (int i = 0; i < numNotes && bufferSize >= 6; i++) {
		unsigned char type = buffer[0];
		unsigned char flags = buffer[1];
		int titleLen = ntohs(*((int16_t *)(buffer + 2)));
		std::string title(buffer + 4, titleLen);
		int contentLen = ntohs(*((int16_t *)(buffer + 4 + titleLen)));
		std::string content(buffer + 6 + titleLen, contentLen);
		m_notes.push_back(new Note(title, type, content, flags));
		buffer += 6 + titleLen + contentLen;
		bufferSize -= 6 + titleLen + contentLen;
	}
	m_valid = true;
}
