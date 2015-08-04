#pragma once

#include "MiscItem.h"

class ItemModItem : public Item {
public:
	ItemModItem(uint32_t id, const std::string &name, int amount, int value, float weight,
		const std::string &icon, const std::string &badge, bool equippable, bool equipped,
		const std::string &effect) : Item(id, name, amount, value, weight, icon, 
			badge, equippable, equipped, effect) {}

	virtual int8_t getItemType() override
	{
		return ITEMTYPE_IMOD;
	}
};

class KeyItem : public MiscItem {
	KeyItem(uint32_t id, const std::string &name, int amount, int value, float weight,
		const std::string &icon, const std::string &badge, bool equippable, bool equipped,
		const std::string &effect) : MiscItem(id, name, amount, value, weight, icon,
			badge, equippable, equipped, effect) {}

	virtual int8_t getItemType() override
	{
		return ITEMTYPE_KEY;
	}
};