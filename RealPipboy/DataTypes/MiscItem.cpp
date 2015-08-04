#include "MiscItem.h"

#include <winsock2.h>

MiscItem::MiscItem(uint32_t id, const std::string &name, int amount, int value, float weight,
	const std::string &icon, const std::string &badge, bool equippable, bool equipped,
	const std::string &effect) : Item(id, name, amount, value, weight, icon,
		badge, equippable, equipped, effect)
{

}

MiscItem::~MiscItem()
{

}

int8_t MiscItem::getItemType()
{
	return ITEMTYPE_MISC;
}

int16_t MiscItem::getDetailsSize()
{
	return 0;
}

void MiscItem::fillItemDetails(char *buffer, size_t bufferSize)
{
}

void MiscItem::readDetailsFromBuffer(const char *buffer, size_t bufferSize)
{
}