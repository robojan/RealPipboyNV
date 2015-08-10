#include "SettingsFormList.h"
#include "Config.h"
#include "nvse/GameData.h"

SettingsFormList::SettingsFormList(std::string name)
{
	load(name);
}

SettingsFormList::SettingsFormList()
{

}

SettingsFormList::~SettingsFormList()
{
}

void SettingsFormList::load(std::string name)
{
	char buffer[256];
	m_list.clear();
	bool endFound = false;
	for (int i = 0; !endFound; i++) {
		sprintf_s(buffer, "%s%02d", name.c_str(), i);
		std::string keyMod = buffer;
		keyMod += "Mod";
		std::string mod = GetConfigOption(name.c_str(), keyMod.c_str());
		if (mod.empty()) {
			mod = "FalloutNV.esm";
		}
		UInt8 modIndex = DataHandler::Get()->GetModIndex(mod.c_str());
		if (modIndex == 0xFF) {
			continue;
		}
		UInt32 id = 0;
		if (!GetConfigOption_UInt32(name.c_str(), buffer, &id)) {
			endFound = true;
		}
		else {
			id = (id & 0xFFFFFF) | (modIndex << 24);
			m_list.insert(id);
		}
	}
}

const std::set<UInt32> & SettingsFormList::getList() const
{
	return m_list;
}

int SettingsFormList::getSize() const
{
	return m_list.size();
}
