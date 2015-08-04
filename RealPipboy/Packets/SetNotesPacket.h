#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"

#include <string>
#include <vector>

#include "../DataTypes/Note.h"

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		2		int16		Number of notes
// Each note:
// 7		1		int8		Note type
// 8		1		int8		Note flags
// 9		2		int16		Length of title
// 7		n		char[]		title
// 7+n		2		int16		content length
// ..		n		char[]		content

class SetNotesPacket :
	public DataPacket
{
public:
	SetNotesPacket(const char *buffer, size_t bufferSize);
	SetNotesPacket(const std::vector<Note *> &notes);
	virtual ~SetNotesPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);
		
private:
	void readPacket(const char *buffer, size_t bufferSize);

	std::vector<Note *> m_notes;
};

