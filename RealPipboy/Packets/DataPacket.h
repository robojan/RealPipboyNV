#pragma once

#include "MessageTypes.h"

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		n		?			data of the packet

class DataPacketHeader {
public:
	DataPacketHeader(const char *buffer, size_t bufferSize);
	DataPacketHeader(MessageType type, size_t size);
	~DataPacketHeader();

	size_t getDataSize();
	MessageType getType();

	size_t fillBuffer(char *buffer, size_t bufferSize);

	static size_t getHeaderSize();
	static bool containsHeader(size_t bufferSize);
	static bool isValidHeader(const char *buffer, size_t bufferSize);
private:
	void readHeader(const char *buffer, size_t bufferSize);
	size_t m_dataSize;
	MessageType m_type;
};

class DataPacket
{
public:
	DataPacket(const char *buffer, size_t bufferSize);
	virtual ~DataPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

	static bool containsCompletePacket(const char *buffer, size_t bufferSize);

	bool isValid();
protected:
	DataPacket(const DataPacketHeader &header);
	DataPacket();

	DataPacketHeader m_header;
	bool m_valid;

private:
	void readPacket(const char *buffer, size_t bufferSize);

	void *m_data;
};

