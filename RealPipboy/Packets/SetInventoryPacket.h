#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include <string>
#include <vector>
#include "../DataTypes/Item.h"

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		2		int16		Num of items
// 7		2		int16		Item length
// 9 + len	len		item		Item info
// etc

// each item:
// 0		1		int8		item type
// 1		4		uint32		item id
// 2		4		int32		item amount
// 6		4		float		item weight
// 10		4		int32		item value
// 14		1		uint8		item flags
// 14		2		int16		item name length
// 16		var		char[]		item name
// ..		2		int16		item icon length
// ..		var		char[]		item icon path
// ..		2		int16		item badge length
// ..		var		char[]		item badge path
// ..		2		int16		item effects length
// ..		var		char[]		item effects
// ..		2		int16		item specific info length
// ..		var		struct		item specific info

#define ITEM_FLAG_EQUIPPABLE 0x01
#define ITEM_FLAG_EQUIPPED 0x02

class SetInventoryPacket :
	public DataPacket
{
public:
	SetInventoryPacket(const char *buffer, size_t bufferSize);
	SetInventoryPacket(const std::vector<Item *> items, bool deleteItems);
	virtual ~SetInventoryPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);
		
private:
	void readPacket(const char *buffer, size_t bufferSize);

	std::vector<Item *> m_items;
	bool m_deleteItems;
};

