#include "GameObjects.h"
#include "GameRTTI.h"
#include "GameExtraData.h"
#include "GameTasks.h"
#include "GameUI.h"

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	static const UInt32 s_TESObject_REFR_init	= 0x0055A2F0;	// TESObject_REFR initialization routine (first reference to s_TESObject_REFR_vtbl)
	static const UInt32	s_Actor_EquipItem		= 0x0088C650;	// maybe, also, would be: 007198E0 for FOSE	4th call from the end of TESObjectREFR::RemoveItem (func5F)
	static const UInt32	s_Actor_UnequipItem		= 0x0088C790;	// maybe, also, would be: 007133E0 for FOSE next sub after EquipItem
	static const UInt32 s_TESObjectREFR__GetContainer	= 0x0055D310;	// First call in REFR::RemoveItem
	static const UInt32 s_TESObjectREFR_Set3D	= 0x005702E0;	// void : (const char*)
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	static const UInt32 s_TESObject_REFR_init	= 0x0055a6b0;	// TESObject_REFR initialization routine (first reference to s_TESObject_REFR_vtbl)
	static const UInt32	s_Actor_EquipItem		= 0x0088c2b0;	// maybe, also, would be: 007198E0 for FOSE	4th call from the end of TESObjectREFR::RemoveItem (func5F)
	static const UInt32	s_Actor_UnequipItem		= 0x0088c3f0;	// maybe, also, would be: 007133E0 for FOSE next sub after EquipItem
	static const UInt32 s_TESObjectREFR__GetContainer	= 0x0055D6B0;	// First call in REFR::RemoveItem
	static const UInt32 s_TESObjectREFR_Set3D	= 0x004E0F80;	// void : (const char*)
#elif EDITOR
#else
#error
#endif

ScriptEventList* TESObjectREFR::GetEventList() const
{
	BSExtraData* xData = extraDataList.GetByType(kExtraData_Script);
	if (xData)
	{
		ExtraScript* xScript = DYNAMIC_CAST(xData, BSExtraData, ExtraScript);
		if (xScript)
			return xScript->eventList;
	}

	return 0;
}

static PlayerCharacter** g_thePlayer = (PlayerCharacter **)0x011DEA3C;

PlayerCharacter* PlayerCharacter::GetSingleton()
{
	return *g_thePlayer;
}

TESContainer* TESObjectREFR::GetContainer()
{
	return (TESContainer*)ThisStdCall(s_TESObjectREFR__GetContainer, this);	// First call in REFR::RemoveItem
}

bool TESObjectREFR::IsMapMarker()
{
	if (baseForm) {
		return baseForm->refID == 0x010;	// Read from the geck. Why is OBSE looking for a pointer ?
	}
	else {
		return false;
	}
}

bool PlayerCharacter::SetSkeletonPath(const char* newPath)
{
	if (!bThirdPerson) {
		// ###TODO: enable in first person
		return false;
	}

	//// store parent of current niNode
	//NiNode* niParent = (NiNode*)(renderState->niNode->m_parent);

	//// set niNode to NULL via BASE CLASS Set3D() method
	//ThisStdCall(s_TESObjectREFR_Set3D, this, NULL);

	//// modify model path
	//if (newPath) {
	//	TESNPC* base = DYNAMIC_CAST(baseForm, TESForm, TESNPC);
	//	base->model.SetPath(newPath);
	//}

	//// create new NiNode, add to parent
	//*(g_bUpdatePlayerModel) = 1;
	//NiNode* newNode = (NiNode*)ThisStdCall(s_PlayerCharacter_GenerateNiNode, this);

	//niParent->AddObject(newNode, 1);
	//*(g_bUpdatePlayerModel) = 0;
	//newNode->SetName("Player");

	//// get and store camera node
	//// ### TODO: pretty this up
	//UInt32 vtbl = *((UInt32*)newNode);
	//UInt32 vfunc = *((UInt32*)(vtbl + 0x58));
	//NiObject* cameraNode = (NiObject*)ThisStdCall(vfunc, newNode, "Camera01");
	//*g_3rdPersonCameraNode = cameraNode;

	//cameraNode = (NiObject*)ThisStdCall(vfunc, (NiNode*)this->firstPersonNiNode, "Camera01");
	//*g_1stPersonCameraNode = cameraNode;

	//Unk_52();

	return true;
}

bool TESObjectREFR::Update3D()
{
	if (this == *g_thePlayer) {
//#ifdef _DEBUG
//		// TESModel* model = DYNAMIC_CAST(baseForm, TESForm, TESModel);
//		// if (model && (*g_thePlayer)->SetSkeletonPath(model->GetModelPath()))
//		{
//			// Update face/head/hair
//			RaceSexMenu* g_raceSexMenu = NULL;
//			InterfaceManager* pIM = InterfaceManager::GetSingleton();
//			if (pIM) {
//				g_raceSexMenu = (RaceSexMenu*)pIM->GetMenuByType(kMenuType_RaceSex);
//				if (!g_raceSexMenu) {
//					g_raceSexMenu = (RaceSexMenu*)pIM->TempMenuByType(kMenuType_RaceSex);
//					if (g_raceSexMenu) {
//						g_raceSexMenu->UpdatePlayerHead();
//						FormHeap_Free(g_raceSexMenu);
//					}
//					return true;
//				}
//			}
//			if (g_raceSexMenu)
//				g_raceSexMenu->UpdatePlayerHead();
//			return true;
//		}
//		//else
//		//	return false;
//#endif
		return true;
	}

	Set3D(NULL, true);
	ModelLoader::GetSingleton()->QueueReference(this, 1, 0);
	return true;
}

TESObjectREFR* TESObjectREFR::Create(bool bTemp)
{
	TESObjectREFR* refr = (TESObjectREFR*)FormHeap_Allocate(sizeof(TESObjectREFR));
	ThisStdCall(s_TESObject_REFR_init, refr);
	if (bTemp)
		CALL_MEMBER_FN(refr, MarkAsTemporary);
	return refr;
}

TESForm* GetPermanentBaseForm(TESObjectREFR* thisObj)	// For LevelledForm, find real baseForm, not temporary one.
{
	ExtraLeveledCreature * pXCreatureData = NULL;

	if (thisObj) {
		pXCreatureData = GetByTypeCast(thisObj->extraDataList, LeveledCreature);
		if (pXCreatureData && pXCreatureData->baseForm) {
			return pXCreatureData->baseForm;
		}
	}
	if (thisObj && thisObj->baseForm) {
		return thisObj->baseForm;
	}
	return NULL;
}

void Actor::EquipItem(TESForm * objType, UInt32 equipCount, ExtraDataList* itemExtraList, UInt32 unk3, bool lockEquip, UInt32 unk5)
{
	ThisStdCall(s_Actor_EquipItem, this, objType, equipCount, itemExtraList, unk3, lockEquip, unk5);
}

void Actor::UnequipItem(TESForm* objType, UInt32 unk1, ExtraDataList* itemExtraList, UInt32 unk3, bool lockUnequip, UInt32 unk5)
{
	ThisStdCall(s_Actor_UnequipItem, this, objType, unk1, itemExtraList, unk3, lockUnequip, unk5);
}

EquippedItemsList Actor::GetEquippedItems()
{
	EquippedItemsList itemList;
	ExtraContainerDataArray outEntryData;
	ExtraContainerExtendDataArray outExtendData;

	ExtraContainerChanges	* xChanges = static_cast <ExtraContainerChanges *>(extraDataList.GetByType(kExtraData_ContainerChanges));
	if(xChanges) {
		UInt32 count = xChanges->GetAllEquipped(outEntryData, outExtendData);
		for (UInt32 i = 0; i < count ; i++)
			itemList.push_back(outEntryData[i]->type);

	}

	return itemList;
}

ExtraContainerDataArray	Actor::GetEquippedEntryDataList()
{
	ExtraContainerDataArray itemArray;
	ExtraContainerExtendDataArray outExtendData;

	ExtraContainerChanges	* xChanges = static_cast <ExtraContainerChanges *>(extraDataList.GetByType(kExtraData_ContainerChanges));
	if(xChanges)
		xChanges->GetAllEquipped(itemArray, outExtendData);

	return itemArray;
}

ExtraContainerExtendDataArray	Actor::GetEquippedExtendDataList()
{
	ExtraContainerDataArray itemArray;
	ExtraContainerExtendDataArray outExtendData;

	ExtraContainerChanges	* xChanges = static_cast <ExtraContainerChanges *>(extraDataList.GetByType(kExtraData_ContainerChanges));
	if(xChanges)
		xChanges->GetAllEquipped(itemArray, outExtendData);

	return outExtendData;
}

