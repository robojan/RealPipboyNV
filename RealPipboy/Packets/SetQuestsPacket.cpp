#include "SetQuestsPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetQuestsPacket::SetQuestsPacket(const char *buffer, size_t bufferSize) :
DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SetQuestsPacket::SetQuestsPacket(const std::vector<Quest *> &list, bool deleteQuests) :
	m_quests(list), m_deleteQuests(deleteQuests)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetQuestsPacket::~SetQuestsPacket()
{
	if (m_deleteQuests) {
		for (std::vector<Quest *>::iterator it = m_quests.begin(); it != m_quests.end(); ++it) {
			delete *it;
		}
	}
}

MessageType SetQuestsPacket::getType()
{
	return MessageType::SETQUESTS;
}

size_t SetQuestsPacket::getSize()
{
	int dataSize = 2;
	for (std::vector<Quest *>::iterator itQuest = m_quests.begin(); 
		itQuest != m_quests.end(); ++itQuest) {
		dataSize += 12 + (*itQuest)->getName().length();
		const std::list<QuestObjective> &objectives = (*itQuest)->getObjectives();
		for (std::list<QuestObjective>::const_iterator itObj = objectives.cbegin();
			itObj != objectives.cend(); ++itObj) {
			dataSize += 12 + (*itObj).getText().length();
			const std::list<QuestObjective::Target> *targets = itObj->getTargets();
			for (std::list<QuestObjective::Target>::const_iterator itTarget = targets->cbegin();
				itTarget != targets->cend(); ++itTarget) {
				dataSize += 4 * 4;
			}
		}
	}

	return DataPacketHeader::getHeaderSize() + dataSize;
}

void SetQuestsPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	*((int16_t *)(buffer + 0)) = htons((uint16_t)m_quests.size());
	buffer += 2;
	bufferSize -= 2;
	for (std::vector<Quest *>::iterator it = m_quests.begin(); it != m_quests.end(); ++it) {
		*((uint32_t *)(buffer + 0)) = htonl((*it)->getQuestID());
		*((uint32_t *)(buffer + 4)) = htonl((*it)->getFlags());
		*((uint16_t *)(buffer + 8)) = htons((uint16_t)(*it)->getName().length());
		memcpy(buffer + 10, (*it)->getName().c_str(), (*it)->getName().length());
		buffer += 10 + (*it)->getName().length();
		bufferSize -= 10 + (*it)->getName().length();

		const std::list<QuestObjective> &objectives = (*it)->getObjectives();

		*((uint16_t *)(buffer + 0)) = htons((uint16_t)objectives.size());
		buffer += 2;
		bufferSize -= 2;

		for (std::list<QuestObjective>::const_iterator itObj = objectives.cbegin();
				itObj != objectives.cend(); ++itObj) {
			*((uint32_t *)(buffer + 0)) = htonl(itObj->getObjectiveID());
			*((uint32_t *)(buffer + 4)) = htonl(itObj->getFlags());
			*((uint16_t *)(buffer + 8)) = htons((uint16_t)itObj->getText().length());
			memcpy(buffer + 10, itObj->getText().c_str(), itObj->getText().length());
			buffer += 10 + itObj->getText().length();
			bufferSize -= 10 + itObj->getText().length();

			const std::list<QuestObjective::Target> *targets = itObj->getTargets();
			*((uint16_t *)(buffer + 0)) = htons((uint16_t)targets->size());
			buffer += 2;
			bufferSize -= 2;

			for (std::list<QuestObjective::Target>::const_iterator itTarget = targets->cbegin();
			itTarget != targets->cend(); ++itTarget) {
				*((uint32_t *)(buffer + 0)) = htonl(itTarget->id);
				*((uint32_t *)(buffer + 4)) = htonf(itTarget->x);
				*((uint32_t *)(buffer + 8)) = htonf(itTarget->y);
				*((uint32_t *)(buffer + 12)) = htonf(itTarget->z);
				buffer += 16;
				bufferSize -= 16;
			}
		}
	}
}

void SetQuestsPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();

	int numQuests = ntohs(*((int16_t *)(buffer + 0)));
	buffer += 2;
	bufferSize -= 2;
	for (int i = 0; i < numQuests && bufferSize >= 10; i++) {
		unsigned int questID = ntohl(*((uint32_t *)(buffer + 0)));
		unsigned int flags = ntohl(*((uint32_t *)(buffer + 4)));
		int nameLen = ntohs(*((uint16_t *)(buffer + 8)));
		std::string name(buffer + 10, nameLen);
		int numObjectives = ntohs(*((uint16_t *)(buffer + 10 + nameLen)));
		buffer += 12 + nameLen;
		bufferSize -= 12 + nameLen;

		Quest *quest = new Quest(questID, name, flags);

		for (int j = 0; j < numObjectives; j++) {
			unsigned int objectiveID = ntohl(*((uint32_t *)(buffer + 0)));
			unsigned int flags = ntohl(*((uint32_t *)(buffer + 4)));
			int textLen = ntohs(*((uint16_t *)(buffer + 8)));
			std::string text(buffer + 10, textLen);
			int numTargets = ntohs(*((uint16_t *)(buffer + 10 + textLen)));
			buffer += 12 + textLen;
			bufferSize -= 12 + textLen;

			QuestObjective objective(objectiveID, text, flags);

			for (int k = 0; k < numTargets; k++) {
				QuestObjective::Target target;
				target.id = ntohl(*((uint32_t *)(buffer + 0)));
				target.x = ntohf(*((uint32_t *)(buffer + 4)));
				target.y = ntohf(*((uint32_t *)(buffer + 4)));
				target.z = ntohf(*((uint32_t *)(buffer + 4)));
				objective.addTarget(target);
			}
			
			quest->addObjective(objective);
		}
	}
	m_valid = true;
}
