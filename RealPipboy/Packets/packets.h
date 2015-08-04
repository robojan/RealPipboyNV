#pragma once

#include "DataPacket.h"
#include "GetFilePacket.h"
#include "HelloPacket.h"
#include "SetGameInfoPacket.h"
#include "SetInventoryPacket.h"
#include "SetNotesPacket.h"
#include "SetPlayerEffectsPacket.h"
#include "SetPlayerInfoPacket.h"
#include "SetQuestsPacket.h"
#include "SetRadioPacket.h"
#include "SetStatsItemsPacket.h"
#include "SetFilePacket.h"
#include "SetWorldInfoPacket.h"
#include "SendFileDataPacket.h"
#include "MessageTypes.h"

inline DataPacket *GetDataPacketFromBuffer(const char *buffer, unsigned int size) {
	DataPacketHeader header = DataPacketHeader(buffer, size);
	switch (header.getType()) {
	case MessageType::HELLO: return new HelloPacket(buffer, size);
	case MessageType::SETPLAYERINFO: return new SetPlayerInfoPacket(buffer, size);
	case MessageType::SETSTATSITEMS: return new SetStatsItemsPacket(buffer, size);
	case MessageType::GETFILE: return new GetFilePacket(buffer, size);
	case MessageType::SETFILE: return new SetFilePacket(buffer, size);
	case MessageType::SETGAMEINFO: return new SetGameInfoPacket(buffer, size);
	case MessageType::SETPLAYEREFFECTS: return new SetPlayerEffectsPacket(buffer, size);
	case MessageType::SETINVENTORY: return new SetInventoryPacket(buffer, size);
	case MessageType::SETQUESTS: return new SetQuestsPacket(buffer, size);
	case MessageType::SETNOTES: return new SetNotesPacket(buffer, size);
	case MessageType::SETRADIO: return new SetRadioPacket(buffer, size);
	case MessageType::SETWORLDINFO: return new SetWorldInfoPacket(buffer, size);
	case MessageType::SENDFILEDATA: return new SendFileDataPacket(buffer, size);
	default: return new DataPacket(buffer, size);
	}
}