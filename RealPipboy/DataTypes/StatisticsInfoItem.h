#pragma once

#include <string>

class StatisticsInfoItem
{
public:
	StatisticsInfoItem(std::string name, std::string icon, std::string description);
	StatisticsInfoItem(std::string name, std::string icon, std::string badge, 
					   std::string description);
	StatisticsInfoItem(std::string name, std::string icon, std::string badge, 
					   std::string description, std::string value, std::string extra);
	~StatisticsInfoItem();

	void setName(const std::string &name);
	void setIcon(const std::string &icon);
	void setBadge(const std::string &badge);
	void setDescription(const std::string &description);
	void setValue(const std::string &value);
	void setExtra(const std::string &extra);
	const std::string &getName() const;
	const std::string &getIcon() const;
	const std::string &getBadge() const;
	const std::string &getDescription() const;
	const std::string &getValue() const;
	const std::string &getExtra() const;

private:
	std::string m_name;
	std::string m_icon;
	std::string m_badge;
	std::string m_description;
	std::string m_value;
	std::string m_extra;
};

