#include "AidItem.h"

#include <winsock2.h>

AidItem::AidItem(uint32_t id, const std::string &name, int amount, int value, float weight,
	const std::string &icon, const std::string &badge, bool equippable, bool equipped,
	const std::string &effect) : Item(id, name, amount, value, weight, icon,
		badge, equippable, equipped, effect) 
{

}

AidItem::~AidItem()
{

}

int8_t AidItem::getItemType()
{
	return ITEMTYPE_AID;
}

int16_t AidItem::getDetailsSize()
{
	return 0;
}

void AidItem::fillItemDetails(char *buffer, size_t bufferSize)
{
}

void AidItem::readDetailsFromBuffer(const char *buffer, size_t bufferSize)
{
}
