#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include <string>
#include <vector>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		1		int8		transfer id
// 6		1		int8		flags
// 7		2		int16		size of string
// 9		len		char[]		name of the file

#define GETFILEFLAG_NOCONVERSION 0x01


class GetFilePacket :
	public DataPacket
{
public:
	GetFilePacket(const char *buffer, size_t bufferSize);
	GetFilePacket(int8_t id, int8_t flags, const std::string &fileName);
	virtual ~GetFilePacket();

	virtual MessageType getType();
	virtual size_t getSize();

	const std::string &getFileName() const;
	int8_t getID() const;
	int8_t getFlags() const;

	virtual void fillBuffer(char *buffer, size_t bufferSize);
		
private:
	void readPacket(const char *buffer, size_t bufferSize);

	std::string m_fileName;
	int8_t m_id;
	int8_t m_flags;
};

