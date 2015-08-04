#include "StatisticsInfoItem.h"

StatisticsInfoItem::StatisticsInfoItem(std::string name, std::string icon, 
									   std::string description) :
m_name(name), m_icon(icon), m_description(description), m_value(""), m_extra(""), m_badge("")
{

}

StatisticsInfoItem::StatisticsInfoItem(std::string name, std::string icon, std::string badge, 
									   std::string description) :
m_name(name), m_icon(icon), m_description(description), m_value(""), m_extra(""), m_badge(badge)
{

}

StatisticsInfoItem::StatisticsInfoItem(std::string name, std::string icon, std::string badge, 
									   std::string description, std::string value, std::string extra) :
m_name(name), m_icon(icon), m_description(description), m_value(value), m_extra(extra), m_badge(badge)
{

}

StatisticsInfoItem::~StatisticsInfoItem()
{

}

void StatisticsInfoItem::setName(const std::string &name)
{
	m_name = name;
}

void StatisticsInfoItem::setIcon(const std::string &icon)
{
	m_icon = icon;
}

void StatisticsInfoItem::setBadge(const std::string &badge)
{
	m_badge = badge;
}

void StatisticsInfoItem::setDescription(const std::string &description)
{
	m_description = description;
}

void StatisticsInfoItem::setValue(const std::string &value)
{
	m_value = value;
}

void StatisticsInfoItem::setExtra(const std::string &extra)
{
	m_extra = extra;
}

const std::string &StatisticsInfoItem::getName() const
{
	return m_name;
}
const std::string &StatisticsInfoItem::getIcon() const
{
	return m_icon;
}

const std::string &StatisticsInfoItem::getBadge() const
{
	return m_badge;
}

const std::string &StatisticsInfoItem::getDescription() const
{
	return m_description;
}

const std::string &StatisticsInfoItem::getValue() const
{
	return m_value;
}

const std::string &StatisticsInfoItem::getExtra() const
{
	return m_extra;
}
