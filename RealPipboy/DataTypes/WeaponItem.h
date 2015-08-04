#pragma once

#include "Item.h"

class WeaponItem :
	public Item
{
public:
	WeaponItem(uint32_t id, std::string name, int amount, int value,
		float weight, std::string icon, std::string badge, bool equippable,
		bool equipped, std::string effect, float dps, float dam, float cnd,  
		int strReq);
	virtual ~WeaponItem();

	virtual int8_t getItemType() override;

	virtual int16_t getDetailsSize() override;

	virtual void fillItemDetails(char *buffer, size_t bufferSize) override;

	virtual void readDetailsFromBuffer(const char *buffer, size_t bufferSize) override;

	float getDPS();
	void setDPS(float dps);
	float getDAM();
	void setDAM(float dam);
	float getCND();
	void setCND(float cnd);
	int getStrReq();
	void setStrReq(int strReq);

protected:
	float m_dps;
	float m_dam;
	float m_condition;
	int m_strReq;
};

