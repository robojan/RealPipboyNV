#pragma once

#include <set>
#include "nvse/prefix.h"
#include "nvse/Utilities.h"

class SettingsFormList
{
public:
	SettingsFormList();
	SettingsFormList(std::string name);
	~SettingsFormList();

	void load(std::string name);

	const std::set<UInt32> &getList() const;
	int getSize() const;
private:
	std::set<UInt32> m_list;
};

