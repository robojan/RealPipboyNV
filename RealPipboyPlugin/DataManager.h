#pragma once

#include "RealPipboy/DataTypes/IDataManager.h"
#include "RealPipboy/DataTypes/StatisticsInfoItem.h"
#include "RealPipboy/DataTypes/StatusEffect.h"
#include "RealPipboy/DataTypes/PlayerEffect.h"
#include "RealPipboy/DataTypes/MapMarker.h"

#include <string>
#include <mutex>

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

	std::vector<StatisticsInfoItem> m_special;
	std::vector<StatisticsInfoItem> m_skills;
	std::vector<StatisticsInfoItem> m_perks;
	std::vector<StatisticsInfoItem> m_stats;
	std::vector<StatusEffect> m_radEffects;
	std::vector<StatusEffect> m_h2oEffects;
	std::vector<StatusEffect> m_fodEffects; 
	std::vector<StatusEffect> m_slpEffects;
	std::vector<PlayerEffect> m_playerEffects;

	std::mutex m_miscStatisticValuesMutex;
	std::vector<int> m_miscStatisticsValues;

	bool m_hardcore;

	std::vector<int> m_updateIds;
	float m_variable;



};

