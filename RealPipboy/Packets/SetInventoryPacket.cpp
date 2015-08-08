#include "SetInventoryPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetInventoryPacket::SetInventoryPacket(const char *buffer, size_t bufferSize) :
	DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SetInventoryPacket::SetInventoryPacket(const std::vector<Item *> items, bool deleteItems) :
	m_items(items), m_deleteItems(deleteItems)
{
	m_header = DataPacketHeader(getType(), getSize()-DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetInventoryPacket::~SetInventoryPacket()
{
	if (m_deleteItems) {
		for (std::vector<Item *>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
			delete *it;
		}
	}
	m_items.clear();
}

MessageType SetInventoryPacket::getType()
{
	return MessageType::SETINVENTORY;
}

size_t SetInventoryPacket::getSize()
{
	int itemsSize = 0;
	for (std::vector<Item *>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
		itemsSize += 2 + 28 + (*it)->getName().length() + (*it)->getIcon().length() +
			(*it)->getBadge().length() + (*it)->getEffect().length() + (*it)->getDetailsSize();
	}

	return DataPacketHeader::getHeaderSize() + 2 + itemsSize;
}

void SetInventoryPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	*((int16_t *)(buffer + 0)) = htons((short)m_items.size());
	buffer += 2;
	bufferSize -= 2;
	for (std::vector<Item *>::iterator it = m_items.begin(); it != m_items.end(); ++it) {
		char *bufferStart = buffer;
		size_t bufferSizeStart = bufferSize;
		int itemSize = 2 + 28 + (*it)->getName().length() + (*it)->getIcon().length() +
			(*it)->getBadge().length() + (*it)->getEffect().length() + (*it)->getDetailsSize();
		if (itemSize > (int)bufferSize)
			break;
		*((uint16_t *)(buffer + 0)) = htons(itemSize);
		buffer[2] = (*it)->getItemType();
		*((uint32_t *)(buffer + 3)) = htonl((*it)->getId());
		*((uint32_t *)(buffer + 7)) = htonl((*it)->getAmount());
		*((uint32_t *)(buffer + 11)) = htonf((*it)->getWeight());
		*((uint32_t *)(buffer + 15)) = htonl((*it)->getValue());
		uint8_t flags = 0;
		flags |= (*it)->isEquippable() ? ITEM_FLAG_EQUIPPABLE : 0;
		flags |= (*it)->isEquipped() ? ITEM_FLAG_EQUIPPED : 0;
		buffer[19] = flags;
		*((uint16_t *)(buffer + 20)) = htons((short)(*it)->getName().length());
		memcpy(buffer + 22, (*it)->getName().c_str(), (*it)->getName().length());
		buffer += 22 + (*it)->getName().length();
		bufferSize -= 22 + (*it)->getName().length();
		*((uint16_t *)(buffer + 0)) = htons((short)(*it)->getIcon().length());
		memcpy(buffer + 2, (*it)->getIcon().c_str(), (*it)->getIcon().length());
		buffer += 2 + (*it)->getIcon().length();
		bufferSize -= 2 + (*it)->getIcon().length();
		*((uint16_t *)(buffer + 0)) = htons((short)(*it)->getBadge().length());
		memcpy(buffer + 2, (*it)->getBadge().c_str(), (*it)->getBadge().length());
		buffer += 2 + (*it)->getBadge().length();
		bufferSize -= 2 + (*it)->getBadge().length();
		*((uint16_t *)(buffer + 0)) = htons((short)(*it)->getEffect().length());
		memcpy(buffer + 2, (*it)->getEffect().c_str(), (*it)->getEffect().length());
		buffer += 2 + (*it)->getEffect().length();
		bufferSize -= 2 + (*it)->getEffect().length();
		*((uint16_t *)(buffer + 0)) = htons((short)(*it)->getDetailsSize());
		(*it)->fillItemDetails(buffer + 2, bufferSize - 2);
		buffer = bufferStart + itemSize;
		bufferSize = bufferSizeStart - itemSize;
	}
}

void SetInventoryPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();

	int numItems = ntohs(*((int16_t *)(buffer + 0)));
	buffer += 2;
	bufferSize -= 2;

	for (int i = 0; i < numItems && bufferSize > 29; i++) {
		const char *bufferStart = buffer;
		size_t bufferSizeStart = bufferSize;

		int itemSize = ntohs(*((uint16_t *)(buffer + 0)));
		int8_t itemType = buffer[2];
		uint32_t id = ntohl(*((uint32_t *)(buffer + 3)));
		int32_t amount = ntohl(*((uint32_t *)(buffer + 7)));
		float weight = ntohf(*((uint32_t *)(buffer + 11)));
		int32_t value = ntohl(*((uint32_t *)(buffer + 15)));
		uint8_t flags = buffer[19];
		int16_t nameLen = ntohs(*((uint16_t *)(buffer + 20)));
		std::string name = std::string(buffer + 22, nameLen);
		buffer += nameLen + 22;
		bufferSize -= nameLen + 22;
		int16_t iconLen = ntohs(*((uint16_t *)(buffer + 0)));
		std::string icon = std::string(buffer + 2, iconLen);
		buffer += iconLen + 2;
		bufferSize -= iconLen + 2;
		int16_t badgeLen = ntohs(*((uint16_t *)(buffer + 0)));
		std::string badge = std::string(buffer + 2, badgeLen);
		buffer += badgeLen + 2;
		bufferSize -= badgeLen + 2;
		int16_t effectsLen = ntohs(*((uint16_t *)(buffer + 0)));
		std::string effects = std::string(buffer + 2, effectsLen);
		buffer += effectsLen + 2;
		bufferSize -= effectsLen + 2;
		int16_t detailsLen = ntohs(*((uint16_t *)(buffer + 0)));
		Item *item = new Item(id, name, amount, value, weight, icon, badge,
			(flags & ITEM_FLAG_EQUIPPABLE) != 0, (flags & ITEM_FLAG_EQUIPPED) != 0, effects);
		item->readDetailsFromBuffer(buffer + 2, bufferSize - 2);
		m_items.push_back(item);
		buffer = bufferStart + itemSize;
		bufferSize = bufferSizeStart - itemSize;
	}
	m_valid = true;
}
