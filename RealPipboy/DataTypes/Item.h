#pragma once

#include <cstdint>
#include <string>

#define ITEMTYPE_UNK 0
#define ITEMTYPE_WEAPON 1
#define ITEMTYPE_AID 2
#define ITEMTYPE_ARMOR 3
#define ITEMTYPE_MISC 4
#define ITEMTYPE_AMMO 5
#define ITEMTYPE_IMOD 6
#define ITEMTYPE_KEY 7
#define ITEMTYPE_

class Item
{
public:
	Item(uint32_t id, const std::string &name, int amount, int value, float weight,
		const std::string &icon, const std::string &badge, bool equippable, bool equipped,
		const std::string &effect);
	~Item();


	virtual int8_t getItemType();
	virtual int16_t getDetailsSize();
	virtual void fillItemDetails(char *buffer, size_t bufferSize);
	virtual void readDetailsFromBuffer(const char *buffer, size_t bufferSize);
	
	uint32_t getId();
	void setId(uint32_t id);
	const std::string &getName();
	void setName(const std::string &name);
	int getAmount();
	void setAmount(int amount);
	int getValue();
	void setValue(int value);
	float getWeight();
	void setWeight(float weigth);
	const std::string &getIcon();
	void setIcon(const std::string &icon);
	const std::string &getBadge();
	void setBadge(const std::string &badge);
	bool isEquippable();
	void setEquippable(bool equippable);
	bool isEquipped();
	void setEquipped(bool equipped);
	const std::string &getEffect();
	void setEffect(const std::string &effect);

protected:
	uint32_t m_id;
	int m_amount;
	int m_value;
	float m_weight;
	std::string m_name;
	std::string m_icon;
	std::string m_badge;
	std::string m_effects;
	bool m_equippable;
	bool m_equipped;
};

