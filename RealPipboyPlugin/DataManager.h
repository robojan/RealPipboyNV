#pragma once

#include "RealPipboy/DataTypes/IDataManager.h"
#include "RealPipboy/DataTypes/StatisticsInfoItem.h"
#include "RealPipboy/DataTypes/StatusEffect.h"
#include "RealPipboy/DataTypes/PlayerEffect.h"

#include <string>
#include <vector>
#include <mutex>

class Item;
class Quest;
class Note;
class MapMarker;

class DataManager :
	public IDataManager
{
public:
	virtual ~DataManager();
	static DataManager &getInstance();
	virtual void registerUpdates(Scheduler &scheduler) override;
	virtual void deregisterUpdates(Scheduler &scheduler) override;

	virtual void update() override;

	virtual std::string getSystemName() override;

	void setStat(int index, int value);
	void setHardcore(bool hardcore);

private:
	DataManager();

	static std::string getModifierExtra(float val);

	static void updateGameInfo(void *);
	static void updatePlayerInfo(void *);
	static void updateSpecial(void *);
	static void updateSkills(void *);
	static void updatePerks(void *);
	static void updateStats(void *);
	static void updateEffects(void *);
	static void updateWorldInfo(void *);
	static void updateInventory(void *);
	static void updateNotes(void *);
	static void updateQuests(void *);
	static void updateMapMarkers(void *);

	static DataManager m_ourInstance;
	std::vector<int> m_updateIds;

	std::mutex m_miscStatisticValuesMutex;
	std::vector<int> m_miscStatisticsValues;

public:
	std::vector<StatisticsInfoItem> m_special;
	std::vector<StatisticsInfoItem> m_skills;
	std::vector<StatisticsInfoItem> m_perks;
	std::vector<StatisticsInfoItem> m_stats;
	std::vector<StatusEffect> m_radEffects;
	std::vector<StatusEffect> m_h2oEffects;
	std::vector<StatusEffect> m_fodEffects; 
	std::vector<StatusEffect> m_slpEffects;
	std::vector<PlayerEffect> m_playerEffects;
	std::vector<Item *> m_inventory;
	std::vector<Note *> m_notes;
	std::vector<Quest *> m_quests;
	std::vector<MapMarker *> m_markers;

	bool m_hardcore;

	uint16_t m_level;
	int16_t m_hp, m_maxHP, m_ap, m_maxAP;
	int32_t m_xp, m_nextXP, m_caps;
	int8_t m_head, m_torso, m_leftArm, m_rightArm, m_leftLeg, m_rightLeg, m_radresist;
	int16_t m_rads, m_dehydration, m_hunger, m_sleep, m_weight, m_maxWeight, m_karma;
	float m_dr, m_dt;

	int m_numStimpacks;
	int m_numDrBag;
	int m_numRadaway;
	int m_numRadX;

};

