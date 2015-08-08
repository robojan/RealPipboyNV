#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"

#include <string>
#include <vector>

#include "../DataTypes/MapMarker.h"

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		2		int16		Number of markers
// Each marker:
// 7		2		int16		Marker type
// 9		2		int16		Marker flags
// 11		4		uint32		id
// 15		4		float		posX
// 19		4		float		posY
// 23		4		float		posZ
// 27		2		int16		Length of name
// 29		n		char[]		name
// ..		2		int16		reputation length
// ..		n		char[]		reputation

class SetMapMarkersPacket :
	public DataPacket
{
public:
	SetMapMarkersPacket(const char *buffer, size_t bufferSize);
	SetMapMarkersPacket(const std::vector<MapMarker *> &markers, bool deleteMarkers);
	virtual ~SetMapMarkersPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

private:
	void readPacket(const char *buffer, size_t bufferSize);

	std::vector<MapMarker *> m_markers;
	bool m_deleteMarkers;
};

