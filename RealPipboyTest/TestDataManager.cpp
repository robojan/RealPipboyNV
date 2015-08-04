#include "TestDataManager.h"

#include "../RealPipboy/Packets/packets.h"
#include "../RealPipboy/Communication/CommunicationManager.h"

#include <Windows.h>

#include <chrono>
#include <ctime>


TestDataManager TestDataManager::m_ourInstance;

TestDataManager::TestDataManager() :
m_variable(0)
{
	m_stats.push_back(StatisticsInfoItem("Quests Completed", "", "", "", "25", ""));
	m_stats.push_back(StatisticsInfoItem("Locations Discovered", "", "", "", "24", ""));
	m_stats.push_back(StatisticsInfoItem("People Killed", "", "", "", "inf", ""));
	m_stats.push_back(StatisticsInfoItem("Creatures Killed", "", "", "", "0", ""));
	m_stats.push_back(StatisticsInfoItem("Locks Picked", "", "", "", "0", ""));
	m_stats.push_back(StatisticsInfoItem("Computers Hacked", "", "", "", "1337", ""));
	m_stats.push_back(StatisticsInfoItem("Stimpacks Taken", "", "", "", "0", ""));
	m_stats.push_back(StatisticsInfoItem("Rad-X Taken", "", "", "", "999", ""));

	m_special.push_back(StatisticsInfoItem("Strength", "textures/interface/icons/pipboyimages"
		"/s.p.e.c.i.a.l/special_strength.dds", "",
		"Strength is a measure of your raw physical power. It affects how much you can carry,"
		" the power of all melee attacks, and your effectiveness with many heavy weapons.",
		"4", "(+)"));
	m_special.push_back(StatisticsInfoItem("Perception", "textures/interface/icons/pipboyimages"
		"/s.p.e.c.i.a.l/special_perception.dds", "",
		"A high Perception grants a bonus to the Explosives, Lockpick and Energy Weapons "
		"skills, and determines when red compass markings appear (which indicate threats).",
		"2", "(-)"));

	m_perks.push_back(StatisticsInfoItem("Perception", "textures/interface/icons/pipboyimages"
		"/s.p.e.c.i.a.l/special_perception.dds", "",
		"A high Perception grants a bonus to the Explosives, Lockpick and Energy Weapons "
		"skills, and determines when red compass markings appear (which indicate threats).",
		"2", "(-)"));

	m_skills.push_back(StatisticsInfoItem("Strength", "textures/interface/icons/pipboyimages"
		"/s.p.e.c.i.a.l/special_strength.dds", "",
		"Strength is a measure of your raw physical power. It affects how much you can carry,"
		" the power of all melee attacks, and your effectiveness with many heavy weapons.",
		"4", "(+)"));

	m_radEffects.push_back(StatusEffect("STR", -1));
	m_radEffects.push_back(StatusEffect("END", -3));
	m_radEffects.push_back(StatusEffect("AGL", -2));
	m_h2oEffects.push_back(StatusEffect("STR", -3));
	m_h2oEffects.push_back(StatusEffect("END", -1));
	m_fodEffects.push_back(StatusEffect("STR", +5));
	m_slpEffects.push_back(StatusEffect("STR", -3));
	m_slpEffects.push_back(StatusEffect("END", -1));
	m_slpEffects.push_back(StatusEffect("STR", +5));
	m_playerEffects.push_back(PlayerEffect("Deadly Rad Poison.", "STR -1, END -3, AGL -2, RAD + 4"));
	m_playerEffects.push_back(PlayerEffect("Minor dehydration", "STR -3, END -1"));
	m_playerEffects.push_back(PlayerEffect("Well fed", "STR +5"));
	m_playerEffects.push_back(PlayerEffect("Sleep deprivation", "STR -3, END -1, STR +5"));
}

TestDataManager::~TestDataManager()
{
}

TestDataManager & TestDataManager::getInstance()
{
	return m_ourInstance;
}

void TestDataManager::registerUpdates(Scheduler &scheduler)
{
	m_updateIds.push_back(scheduler.addPeriodic(TestDataManager::updateGameInfo, 1000));
	m_updateIds.push_back(scheduler.addPeriodic(TestDataManager::updatePlayerInfo, 200));
	m_updateIds.push_back(scheduler.addPeriodic(TestDataManager::updateSpecial, 5000));
	m_updateIds.push_back(scheduler.addPeriodic(TestDataManager::updateSkills, 5000));
	m_updateIds.push_back(scheduler.addPeriodic(TestDataManager::updatePerks, 5000));
	m_updateIds.push_back(scheduler.addPeriodic(TestDataManager::updateStats, 5000));
	m_updateIds.push_back(scheduler.addPeriodic(TestDataManager::updateEffects, 2000));
	m_updateIds.push_back(scheduler.addPeriodic(TestDataManager::updateWorldInfo, 500));
}

void TestDataManager::deregisterUpdates(Scheduler &scheduler)
{
	for (std::vector<int>::iterator it = m_updateIds.begin(); it != m_updateIds.end(); ++it) {
		scheduler.removeTask(*it);
	}
}

void TestDataManager::updateGameInfo(void *)
{
	CommunicationManager::getInstance().sendPacket(new SetGameInfoPacket(0, true));
}

void TestDataManager::updatePlayerInfo(void *)
{
	TestDataManager &i = TestDataManager::getInstance();
	i.m_variable += 0.2f * 1.0f/5.0f;
	if (i.m_variable > 1.0f) {
		i.m_variable -= 1.0f;
	}
	float v = i.m_variable;
	CommunicationManager::getInstance().sendPacket(new SetPlayerInfoPacket((short)(v*33), 
		(short)(v*133), 234, (short)(v*34), 212, (short)(v*12341), 111811111, (char)(v*50), 0,
		10, 100, 75, 23, 65, (short)(v*122), 1000, 0, 1000, 500, 1000, 100, 1000, 135, 250, 0, 2,
		9999999, 0, "Robojan", 0,0,0,0,0,0));
}

void TestDataManager::updateSpecial(void *)
{
	TestDataManager &i = TestDataManager::getInstance();
	CommunicationManager::getInstance().sendPacket(new SetStatsItemsPacket(0, i.m_special));
}

void TestDataManager::updateSkills(void *)
{
	TestDataManager &i = TestDataManager::getInstance();
	CommunicationManager::getInstance().sendPacket(new SetStatsItemsPacket(1, i.m_skills));
}

void TestDataManager::updatePerks(void *)
{
	TestDataManager &i = TestDataManager::getInstance();
	CommunicationManager::getInstance().sendPacket(new SetStatsItemsPacket(2, i.m_perks));
}

void TestDataManager::updateStats(void *)
{
	TestDataManager &i = TestDataManager::getInstance();
	CommunicationManager::getInstance().sendPacket(new SetStatsItemsPacket(3, i.m_stats));
}

void TestDataManager::updateEffects(void *)
{
	TestDataManager &i = TestDataManager::getInstance();
	CommunicationManager::getInstance().sendPacket(new SetPlayerEffectsPacket(i.m_radEffects, 
		i.m_h2oEffects, i.m_fodEffects, i.m_slpEffects, i.m_playerEffects));
}

void TestDataManager::updateWorldInfo(void *)
{
	time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm *timeInfo = localtime(&tt);

	CommunicationManager::getInstance().sendPacket(new SetWorldInfoPacket(timeInfo->tm_year + 1900, timeInfo->tm_mon, timeInfo->tm_mday,
		timeInfo->tm_hour, timeInfo->tm_min, "Wasteland", 2048,2048,-35,35,25,-25,1,0,0, 
		"textures\\interface\\worldmap\\wasteland_nv_1024_no_map.dds"));

}

void TestDataManager::update()
{
	
}

std::string TestDataManager::getSystemName()
{
	char nameBuffer[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD bufferLen = MAX_COMPUTERNAME_LENGTH + 1;
	if (!GetComputerNameA(nameBuffer, &bufferLen)) {
		return "Pipboy 3000";
	}
	return std::string(nameBuffer);
}