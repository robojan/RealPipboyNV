#pragma once
#include "RealPipboy\Communication\CommunicationManager.h"
#include "RealPipboy\DataTypes\IDataManager.h"

class ActionHandler :
	public IPacketHandler
{
public:
	ActionHandler();
	~ActionHandler();

	virtual void handlePacket(DataPacket *packet) override;

	void actionUseStimpack();
	void actionUseDrBag();
	void actionUseRadAway();
	void actionUseRadX();
	void actionEquipItem(UInt32 refID, bool noEquip, bool hideMessage);
	void actionUnequipItem(UInt32 refID, bool noEquip, bool hideMessage);
	void actionFastTravel(UInt32 refID);
	void actionSetActiveQuest(UInt32 refID);
	void actionTuneRadio(UInt32 refID);
	void actionDropItem(UInt32 refID, int count);
private:
};

