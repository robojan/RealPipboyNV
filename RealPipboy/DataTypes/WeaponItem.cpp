#include "WeaponItem.h"

#include <winsock2.h>

WeaponItem::WeaponItem(uint32_t id, std::string name, int amount, int value,
	float weight, std::string icon, std::string badge, bool equippable,
	bool equipped, std::string effect, float dps, float dam, float cnd, 
	int strReq, std::string ammo) :
	Item(id, name, amount, value, weight, icon, badge, equippable, equipped, effect),
	m_dps(dps), m_dam(dam), m_condition(cnd), m_strReq(strReq), m_ammo(ammo)
{
}

WeaponItem::~WeaponItem()
{
}

int8_t WeaponItem::getItemType()
{
	return ITEMTYPE_WEAPON;
}

int16_t WeaponItem::getDetailsSize()
{
	return 18 + (int16_t)m_ammo.length();
}

void WeaponItem::fillItemDetails(char *buffer, size_t bufferSize)
{
	(*(uint32_t *)(buffer + 0)) = htonf(m_dps);
	(*(uint32_t *)(buffer + 4)) = htonf(m_dam);
	(*(uint32_t *)(buffer + 8)) = htonf(m_condition);
	(*(uint32_t *)(buffer + 12)) = htonl(m_strReq);
	(*(uint16_t *)(buffer + 16)) = htons((short)m_ammo.length());
	memcpy(buffer + 18, m_ammo.c_str(), m_ammo.length());
}

void WeaponItem::readDetailsFromBuffer(const char *buffer, size_t bufferSize)
{
	m_dps = ntohf(*(uint32_t *)(buffer + 0));
	m_dam = ntohf(*(uint32_t *)(buffer + 4));
	m_condition = ntohf(*(uint32_t *)(buffer + 8));
	m_strReq = ntohl(*(uint32_t *)(buffer + 12));
	int ammoLen = ntohs(*(uint16_t *)(buffer + 16));
	m_ammo = std::string(buffer + 18, ammoLen);

}

float WeaponItem::getDPS()
{
	return m_dps;
}

void WeaponItem::setDPS(float dps)
{
	m_dps = dps;
}

float WeaponItem::getDAM()
{
	return m_dam;
}

void WeaponItem::setDAM(float dam)
{
	m_dam = dam;
}

float WeaponItem::getCND()
{
	return m_condition;
}

void WeaponItem::setCND(float cnd)
{
	m_condition = cnd;
}

int WeaponItem::getStrReq()
{
	return m_strReq;
}

void WeaponItem::setStrReq(int strReq)
{
	m_strReq = strReq;
}

const std::string & WeaponItem::getAmmo()
{
	return m_ammo;
}

void WeaponItem::setAmmo(const std::string &ammo)
{
	m_ammo = ammo;
}
