#pragma once

#include "Item.h"

class ApparelItem :
	public Item
{
public:
	ApparelItem(uint32_t id, const std::string &name, int amount, int value,
		float weight, const std::string &icon, const std::string &badge, bool equippable,
		bool equipped, const std::string &effect, float dt, float dr, float cnd);
	virtual ~ApparelItem();

	virtual int8_t getItemType() override;

	virtual int16_t getDetailsSize() override;

	virtual void fillItemDetails(char *buffer, size_t bufferSize) override;

	virtual void readDetailsFromBuffer(const char *buffer, size_t bufferSize) override;

	float getDT();
	void setDT(float dt);
	float getDR();
	void setDR(float dr);
	float getCND();
	void setCND(float cnd);

protected:
	float m_dt;
	float m_dr;
	float m_condition;
};

