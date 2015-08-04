#include "AmmoItem.h"

#include <winsock2.h>

AmmoItem::AmmoItem(uint32_t id, const std::string &name, int amount, int value, float weight,
	const std::string &icon, const std::string &badge, bool equippable, bool equipped,
	const std::string &effect) : Item(id, name, amount, value, weight, icon,
		badge, equippable, equipped, effect)
{

}

AmmoItem::~AmmoItem()
{

}

int8_t AmmoItem::getItemType()
{
	return ITEMTYPE_AMMO;
}

int16_t AmmoItem::getDetailsSize()
{
	return 0;
}

void AmmoItem::fillItemDetails(char *buffer, size_t bufferSize)
{
}

void AmmoItem::readDetailsFromBuffer(const char *buffer, size_t bufferSize)
{
}
