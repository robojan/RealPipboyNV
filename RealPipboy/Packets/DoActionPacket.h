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
// 5		4		int32		action id
// 9		4		uint32		param1
// 13		4		uint32		param2
// 17		4		uint32		param3
// 21		4		uint32		param4

#define DOACTION_USESTIMPACK 0
#define DOACTION_USEDRBAG 1
#define DOACTION_USERADAWAY 2
#define DOACTION_USERADX 3
#define DOACTION_EQUIPITEM 4 // RefID, noUnequip, hideMessage
#define DOACTION_UNEQUIPITEM 5 // RefID, noEquip, hideMessage
#define DOACTION_FASTTRAVEL 6 // RefID
#define DOACTION_SETACTIVEQUEST 7 // RefID
#define DOACTION_TUNERADIO 8 // RefID

class DoActionPacket :
	public DataPacket
{
public:
	DoActionPacket(const char *buffer, size_t bufferSize);
	DoActionPacket(int32_t id, uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4);
	virtual ~DoActionPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	int32_t getID() const;
	uint32_t getParam(int index) const;

	virtual void fillBuffer(char *buffer, size_t bufferSize);

private:
	void readPacket(const char *buffer, size_t bufferSize);

	int32_t m_id;
	uint32_t m_param[4];
};

