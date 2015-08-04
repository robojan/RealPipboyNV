#include "Item.h"

Item::Item(uint32_t id, const std::string &name, int amount, int value, float weight,
	const std::string &icon, const std::string &badge, bool equippable, bool equipped,
	const std::string &effect) :
	m_id(id), m_name(name), m_amount(amount), m_value(value), m_weight(weight),
	m_icon(icon), m_badge(badge), m_equippable(equippable), m_equipped(equipped),
	m_effects(effect)
{
}

Item::~Item()
{
}

int8_t Item::getItemType()
{
	return 0;
}

int16_t Item::getDetailsSize()
{
	return 0;
}

void Item::fillItemDetails(char *buffer, size_t bufferSize)
{

}

void Item::readDetailsFromBuffer(const char *buffer, size_t bufferSize)
{

}

uint32_t Item::getId()
{
	return m_id;
}

void Item::setId(uint32_t id)
{
	m_id = id;
}

const std::string & Item::getName()
{
	return m_name;
}

void Item::setName(const std::string &name)
{
	m_name = name;
}

int Item::getAmount()
{
	return m_amount;
}

void Item::setAmount(int amount)
{
	m_amount = amount;
}

int Item::getValue()
{
	return m_value;
}

void Item::setValue(int value)
{
	m_value = value;
}

float Item::getWeight()
{
	return m_weight;
}

void Item::setWeight(float weigth)
{
	m_weight = weigth;
}

const std::string & Item::getIcon()
{
	return m_icon;
}

void Item::setIcon(const std::string &icon)
{
	m_icon = icon;
}

const std::string & Item::getBadge()
{
	return m_badge;
}

void Item::setBadge(const std::string &badge)
{
	m_badge = badge;
}

bool Item::isEquippable()
{
	return m_equippable;
}

void Item::setEquippable(bool equippable)
{
	m_equippable = equippable;
}

bool Item::isEquipped()
{
	return m_equipped;
}

void Item::setEquipped(bool equipped)
{
	m_equipped = equipped;
}

const std::string & Item::getEffect()
{
	return m_effects;
}

void Item::setEffect(const std::string &effect)
{
	m_effects = effect;
}
