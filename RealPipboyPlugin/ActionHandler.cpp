#include "ActionHandler.h"
#include "DataManager.h"
#include "nvse/GameObjects.h"
#include "nvse/GameScript.h"
#include "nvse/GameAPI.h"
#include "nvse/gameRTTI.h"
#include "nvse/GameForms.h"
#include "nvse/GameExtraData.h"
#include "nvse/GameBSExtraData.h"
#include "nvse/GameData.h"

static char commandBuf[250];

ActionHandler::ActionHandler()
{
	CommunicationManager::getInstance().registerPacketHandler(MessageType::DOACTION, this);
}


ActionHandler::~ActionHandler()
{
}

void ActionHandler::handlePacket(DataPacket *p)
{
	DoActionPacket *packet = dynamic_cast<DoActionPacket *>(p);
	union {
		float fparams[4];
		uint32_t iparams[4];
	};
	switch (packet->getID()) {
	case DOACTION_USESTIMPACK:
		actionUseStimpack();
		break;
	case DOACTION_USEDRBAG:
		actionUseDrBag();
		break;
	case DOACTION_USERADAWAY:
		actionUseRadAway();
		break;
	case DOACTION_USERADX:
		actionUseRadX();
		break;
	case DOACTION_EQUIPITEM:
		actionEquipItem(packet->getParam(0), packet->getParam(1) != 0, 
			packet->getParam(2) != 0);
		break;
	case DOACTION_UNEQUIPITEM:
		actionUnequipItem(packet->getParam(0), packet->getParam(1) != 0,
			packet->getParam(2) != 0);
		break;
	case DOACTION_FASTTRAVEL:
		actionFastTravel(packet->getParam(0));
		break;
	case DOACTION_SETACTIVEQUEST:
		actionSetActiveQuest(packet->getParam(0));
		break;
	case DOACTION_TUNERADIO:
		actionTuneRadio(packet->getParam(0));
		break;
	default:
		_ERROR("Unknown Do Action packet received. Action: %d", packet->getID());
	}
}

void ActionHandler::actionUseStimpack()
{
	DataManager &dm = DataManager::getInstance();
	BGSDefaultObjectManager *defObj = BGSDefaultObjectManager::GetSingleton();
	if (dm.m_numStimpacks <= 0 || (dm.m_hp >= dm.m_maxHP)) {
		return;
	}
	//PlayerCharacter *player = PlayerCharacter::GetSingleton();
	//player->EquipItem(defObj->defaultObjects.asStruct.Stimpak, 1, NULL, )
	sprintf_s(commandBuf, "player.equipitem %x 0 1", 
		defObj->defaultObjects.asStruct.Stimpak->refID);
	Script::RunScriptLine(commandBuf);
}

void ActionHandler::actionUseDrBag()
{
	DataManager &dm = DataManager::getInstance();
	BGSDefaultObjectManager *defObj = BGSDefaultObjectManager::GetSingleton();
	if (dm.m_numDrBag <= 0 ) {
		return;
	}
	sprintf_s(commandBuf, "player.equipitem %x 0 1", 
		defObj->defaultObjects.asStruct.DoctorsBag->refID);
	Script::RunScriptLine(commandBuf);
}

void ActionHandler::actionUseRadAway()
{
	DataManager &dm = DataManager::getInstance();
	BGSDefaultObjectManager *defObj = BGSDefaultObjectManager::GetSingleton();
	if (dm.m_numDrBag <= 0) {
		return;
	}
	sprintf_s(commandBuf, "player.equipitem %x 0 1",
		defObj->defaultObjects.asStruct.RadAway->refID);
	Script::RunScriptLine(commandBuf);
}

void ActionHandler::actionUseRadX()
{
	DataManager &dm = DataManager::getInstance();
	BGSDefaultObjectManager *defObj = BGSDefaultObjectManager::GetSingleton();
	if (dm.m_numRadX <= 0) {
		return;
	}
	sprintf_s(commandBuf, "player.equipitem %x 0 1",
		defObj->defaultObjects.asStruct.RadX->refID);
	Script::RunScriptLine(commandBuf);
}

void ActionHandler::actionEquipItem(UInt32 refID, bool noEquip, bool hideMessage)
{
	sprintf_s(commandBuf, "player.equipitem %x %d %d", refID, noEquip ? 1 : 0, hideMessage ? 1 : 0);
	Script::RunScriptLine(commandBuf);
}

void ActionHandler::actionUnequipItem(UInt32 refID, bool noEquip, bool hideMessage)
{
	sprintf_s(commandBuf, "player.unequipitem %x %d %d", refID, noEquip ? 1 : 0, hideMessage ? 1 : 0);
	Script::RunScriptLine(commandBuf);
}

void ActionHandler::actionFastTravel(UInt32 refID)
{
	PlayerCharacter *player = PlayerCharacter::GetSingleton();
	TESObjectREFR *marker = (TESObjectREFR *)LookupFormByID(refID);
	if (marker == NULL) return;
	ExtraMapMarker *markerData = (ExtraMapMarker *)
		marker->extraDataList.GetByType(kExtraData_MapMarker);
	if (markerData == NULL || !markerData->CanTravel()) return;
	_MESSAGE("Flags %08x, %08x, %08x", player->unk650[6], player->unk650[7], player->unk650[8]);
	if (player->unk650[7] & 0x100 && !player->parentCell->IsInterior()) {
		_VMESSAGE("Fast travel");
		ExtraReferencePointer *ftp = (ExtraReferencePointer *)
			marker->extraDataList.GetByType(kExtraData_LinkedRef);
		if (ftp == NULL || ftp->refr == NULL) {
			_ERROR("Fast travel heading not found");
		}
		sprintf_s(commandBuf, "player.moveto %x 0 0 0", ftp->refr->refID);
		Script::RunScriptLine(commandBuf);
	}
	
}

void ActionHandler::actionSetActiveQuest(UInt32 refID)
{
	TESQuest *quest = DYNAMIC_CAST(LookupFormByID(refID), TESForm, TESQuest);
	if (quest == NULL) {
		return;
	}
	sprintf_s(commandBuf, "ForceActiveQuest %x", quest->refID);
	Script::RunScriptLine(commandBuf);
}

void ActionHandler::actionTuneRadio(UInt32 refID)
{
	if (refID != 0) {
		TESObjectREFR *radio = DYNAMIC_CAST(LookupFormByID(refID), TESForm, TESObjectREFR);
		if (radio == NULL)
			return;
		sprintf_s(commandBuf, "pipboyRadio off");
		Script::RunScriptLine(commandBuf);
		sprintf_s(commandBuf, "pipboyRadio on");
		Script::RunScriptLine(commandBuf);
		sprintf_s(commandBuf, "pipboyRadio tune %x", radio->refID);
		Script::RunScriptLine(commandBuf);
	}
	else {
		sprintf_s(commandBuf, "pipboyRadio off");
		Script::RunScriptLine(commandBuf);
	}
}

