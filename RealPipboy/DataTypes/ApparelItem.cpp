#include "ApparelItem.h"

#include <winsock2.h>

ApparelItem::ApparelItem(uint32_t id, const std::string &name, int amount, int value, 
	float weight, const std::string &icon, const std::string &badge, bool equippable,
	bool equipped, const std::string &effect, float dt, float dr, float cnd) :
	Item(id, name, amount, value, weight, icon, badge, equippable, equipped, effect),
	m_dt(dt), m_dr(dr), m_condition(cnd)
{

}

ApparelItem::~ApparelItem()
{

}

int8_t ApparelItem::getItemType()
{
	return ITEMTYPE_ARMOR;
}

int16_t ApparelItem::getDetailsSize()
{
	return 12;
}

void ApparelItem::fillItemDetails(char *buffer, size_t bufferSize)
{
	(*(uint32_t *)(buffer + 0)) = htonf(m_dr);
	(*(uint32_t *)(buffer + 4)) = htonf(m_dt);
	(*(uint32_t *)(buffer + 8)) = htonf(m_condition);
}

void ApparelItem::readDetailsFromBuffer(const char *buffer, size_t bufferSize)
{
	m_dr = ntohf(*(uint32_t *)(buffer + 0));
	m_dt = ntohf(*(uint32_t *)(buffer + 4));
	m_condition = ntohf(*(uint32_t *)(buffer + 8));
}

float ApparelItem::getDT()
{
	return m_dt;
}

void ApparelItem::setDT(float dt)
{
	m_dt = dt;
}

float ApparelItem::getDR()
{
	return m_dr;
}

void ApparelItem::setDR(float dr)
{
	m_dr = dr;
}

float ApparelItem::getCND()
{
	return m_condition;
}

void ApparelItem::setCND(float cnd)
{
	m_condition = cnd;
}
