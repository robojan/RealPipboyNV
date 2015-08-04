
#include "nvse/PluginAPI.h"
#include "nvse/CommandTable.h"
#include "nvse/GameAPI.h"
#include "nvse/ParamInfos.h"
#include "nvse/GameObjects.h"
#include "DataManager.h"
#include "common/IPrefix.h"
#include "RealPipboy/RealPipboy.h"
#include "DataManager.h"
#include "FOResourceManager.h"
#include "FileTransferHandler.h"

#include <string>
#include <process.h>


#ifdef NOGORE
IDebugLog		gLog("real_pipboy_ng.log");
#else
IDebugLog		gLog("real_pipboy.log");
#endif

PluginHandle	g_pluginHandle = kPluginHandle_Invalid;

NVSEMessagingInterface* g_msg;
NVSEInterface * SaveNVSE;
NVSECommandTableInterface * g_cmdTable;
const CommandInfo * g_TFC;
NVSEScriptInterface* g_script;
#define ExtractArgsEx(...) g_script->ExtractArgsEx(__VA_ARGS__)
#define ExtractFormatStringArgs(...) g_script->ExtractFormatStringArgs(__VA_ARGS__)


uintptr_t g_mainThreadHandle;
RealPipboy gPipboy(&DataManager::getInstance());
bool gRunning = false;
unsigned int __stdcall mainThread(void *){
	_MESSAGE("Starting main thread");
	std::string zlibDll = GetFalloutDirectory() + "Data\\zlib1.dll";
	LoadLibrary(zlibDll.c_str());

	gPipboy.init();

	FOResourceManager::getSingleton()->load(GetFalloutDirectory() + "Data");

	FileTransferHandler fileTransferHandler;

	_MESSAGE("Starting server");
	gPipboy.makeConnectable(true);
	while (gRunning) {
		gPipboy.update();
	}
	_MESSAGE("Stopping server");
	gPipboy.makeConnectable(false);
	return 0;
}


void MessageHandler(NVSEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
		case NVSEMessagingInterface::kMessage_NewGame:
			_MESSAGE("Received new game message");
			gPipboy.setInGame(true);
			break;
		case NVSEMessagingInterface::kMessage_LoadGame:
			_MESSAGE("Received load game message with file path %s", msg->data);
			break;
		case NVSEMessagingInterface::kMessage_SaveGame:
			_MESSAGE("Received save game message with file path %s", msg->data);
			break;
		case NVSEMessagingInterface::kMessage_PreLoadGame:
			_MESSAGE("Received pre load game message with file path %s", msg->data);
			break;
		case NVSEMessagingInterface::kMessage_PostLoadGame:
			_MESSAGE("Received post load game message: %s", msg->data ? "Error/Unkwown" : "OK");
			gPipboy.setInGame(msg->data != NULL);
			break;
		case NVSEMessagingInterface::kMessage_PostLoad:
			_MESSAGE("Received post load plugins message");
			break;
		case NVSEMessagingInterface::kMessage_PostPostLoad:
			_MESSAGE("Received post post load plugins message");
			break;
		case NVSEMessagingInterface::kMessage_ExitGame:
			_MESSAGE("Received exit game message");
			gRunning = false;
			gPipboy.setInGame(false);
			break;
		case NVSEMessagingInterface::kMessage_ExitGame_Console:
			_MESSAGE("Received exit game via console qqq command message");
			gRunning = false;
			gPipboy.setInGame(false);
			break;
		case NVSEMessagingInterface::kMessage_ExitToMainMenu:
			_MESSAGE("Received exit game to main menu message");
			gPipboy.setInGame(false);
			break;
		case NVSEMessagingInterface::kMessage_Precompile:
			_MESSAGE("Received precompile message with script at %08x", msg->data);
			break;
		case NVSEMessagingInterface::kMessage_RuntimeScriptError:
			_MESSAGE("Received runtime script error message %s", msg->data);
			break;
		default:
			_MESSAGE("Received unknown message %d", msg->type);
			break;
	}
}

#ifdef RUNTIME

bool Cmd_RPUpdateMiscStat_Execute(COMMAND_ARGS)
{
	*result = 1;

	int stat;
	int value;

	if (!ExtractArgs(EXTRACT_ARGS, &stat, &value))
		return true;

	DataManager::getInstance().setStat(stat, value);

	return true;
}

bool Cmd_RPUpdateHardcore_Execute(COMMAND_ARGS)
{
	*result = 1;

	int hardcore;

	if (!ExtractArgs(EXTRACT_ARGS, &hardcore))
		return true;

	DataManager::getInstance().setHardcore(hardcore != 0);

	return true;
}

#endif

static ParamInfo kParams_RPUpdateMiscStat[2] = {
	{ "Statistic index", kParamType_Integer, 0},
	{ "value" , kParamType_Integer, 0}
};

static ParamInfo kParams_RPUpdateHardcore[1] = {
	{ "hardcore", kParamType_Integer, 0 },
};

DEFINE_COMMAND_PLUGIN(RPUpdateMiscStat, "Set a miscellaneous statistic on the app", 0, 2, kParams_RPUpdateMiscStat);
DEFINE_COMMAND_PLUGIN(RPUpdateHardcore, "Set hardcore mode on the app", 0, 1, kParams_RPUpdateHardcore);

extern "C" {

bool NVSEPlugin_Query(const NVSEInterface * nvse, PluginInfo * info)
{
	_MESSAGE("query");

	// fill out the info structure
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "real_pipboy";
	info->version = 2;

	// version checks
	if(nvse->nvseVersion < NVSE_VERSION_INTEGER)
	{
		_ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, NVSE_VERSION_INTEGER);
		return false;
	}

	if(!nvse->isEditor)
	{
		if(nvse->runtimeVersion < RUNTIME_VERSION_1_4_0_525)
		{
			_ERROR("incorrect runtime version (got %08X need at least %08X)", nvse->runtimeVersion, RUNTIME_VERSION_1_4_0_525);
			return false;
		}

#ifdef NOGORE
		if(!nvse->isNogore)
		{
			_ERROR("incorrect runtime edition (got %08X need %08X (nogore))", nvse->isNogore, 1);
			return false;
		}
#else
		if(nvse->isNogore)
		{
			_ERROR("incorrect runtime edition (got %08X need %08X (standard))", nvse->isNogore, 0);
			return false;
		}
#endif
	}
	else
	{
		if(nvse->editorVersion < CS_VERSION_1_4_0_518)
		{
			_ERROR("incorrect editor version (got %08X need at least %08X)", nvse->editorVersion, CS_VERSION_1_4_0_518);
			return false;
		}
#ifdef NOGORE
		_ERROR("Editor only uses standard edition, closing.");
		return false;
#endif
	}

	// version checks pass

	return true;
}

bool NVSEPlugin_Load(const NVSEInterface * nvse)
{
	_MESSAGE("load");

	g_pluginHandle = nvse->GetPluginHandle();

	// save the NVSEinterface in case we need it later
	SaveNVSE = (NVSEInterface *)nvse;

	// register to receive messages from NVSE
	NVSEMessagingInterface* msgIntfc = (NVSEMessagingInterface*)nvse->QueryInterface(kInterface_Messaging);
	msgIntfc->RegisterListener(g_pluginHandle, "NVSE", MessageHandler);
	g_msg = msgIntfc;

	g_script = (NVSEScriptInterface*)nvse->QueryInterface(kInterface_Script);
	g_cmdTable = (NVSECommandTableInterface *)nvse->QueryInterface(kInterface_CommandTable);

	/***************************************************************************
	 *	
	 *	READ THIS!
	 *	
	 *	Before releasing your plugin, you need to request an opcode range from
	 *	the NVSE team and set it in your first SetOpcodeBase call. If you do not
	 *	do this, your plugin will create major compatibility issues with other
	 *	plugins, and will not load in release versions of NVSE. See
	 *	nvse_readme.txt for more information.
	 *	
	 **************************************************************************/

	// register commands
	nvse->SetOpcodeBase(0x3000);
	nvse->RegisterCommand(&kCommandInfo_RPUpdateMiscStat);
	nvse->RegisterCommand(&kCommandInfo_RPUpdateHardcore);

	if (!nvse->isEditor) {
		gRunning = true;
		g_mainThreadHandle = _beginthreadex(NULL, 0, mainThread, NULL, NULL, NULL);
	}

	return true;
}

};
