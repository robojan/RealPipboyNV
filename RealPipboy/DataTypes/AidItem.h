#pragma once

#include "Item.h"

class AidItem :
	public Item
{
public:
	AidItem(uint32_t id, const std::string &name, int amount, int value, float weight,
		const std::string &icon, const std::string &badge, bool equippable, bool equipped,
		const std::string &effect);
	virtual ~AidItem();

	virtual int8_t getItemType() override;

	virtual int16_t getDetailsSize() override;

	virtual void fillItemDetails(char *buffer, size_t bufferSize) override;

	virtual void readDetailsFromBuffer(const char *buffer, size_t bufferSize) override;

protected:
};

