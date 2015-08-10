// RealPipboyTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common/IPrefix.h"
#include "RealPipboy/RealPipboy.h"
#include "TestDataManager.h"
#include "RealPipboyPlugin/FOResourceManager.h"

#define DEFAULT_PORT "28115"

#include <iostream>
#include <string>

#include "RealPipboyPlugin/FOResourceStream.h"

void showMessage(bool error, const char *msg) {
	if (error) {
		_ERROR("Error: %s", msg);
	}
	else {
		_MESSAGE(msg);
	}
}

IDebugLog gLog("RealPipboyTest.log");
RealPipboy gPipboy(&TestDataManager::getInstance(), showMessage);
bool gRunning = true;


int _tmain(int argc, _TCHAR* argv[])
{
	LoadLibrary("data\\zlib1.dll");
	_MESSAGE("RealPipboyTest is starting");

	gPipboy.init();

	FOResourceManager *resources = FOResourceManager::getSingleton();
	uint64_t hash1 = resources->hash("textures\\clutter\\snowglobes", "");
	uint64_t hash2 = resources->hash("", "snowglobenelisafb_d.dds");
	resources->load("C:\\SteamGames\\steamapps\\Common\\Fallout New Vegas\\Data");
	
	FOResourceStream fileStream("textures\\interface\\icons\\mood\\glow_messages_happy_big.dds");
	UInt64 fileLen = fileStream.GetLength();
	UInt8 *tempBuffer = new UInt8[fileLen];
	fileStream.ReadBuf(tempBuffer, fileLen);
	delete[] tempBuffer;

	gPipboy.makeConnectable(true);

	while (gRunning) {
		gPipboy.update();
	}

	gPipboy.makeConnectable(false);

	return 0;
}
