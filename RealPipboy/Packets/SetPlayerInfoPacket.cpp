#include "SetPlayerInfoPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetPlayerInfoPacket::SetPlayerInfoPacket(const char *buffer, size_t bufferSize) :
DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SetPlayerInfoPacket::SetPlayerInfoPacket(int16_t level, int16_t hp, int16_t maxHP, int16_t ap, int16_t maxAP,
	int32_t xp, int32_t maxXP, int8_t head, int8_t torso, int8_t leftArm,
	int8_t rightArm, int8_t leftLeg, int8_t rightLeg, int8_t radResist,
	int16_t rads, int16_t maxRads, int16_t h2o, int16_t maxH2o,
	int16_t fod, int16_t maxFod, int16_t slp, int16_t maxSlp, int16_t weight,
	int16_t maxWeight, float dr, float dt, int32_t caps, int16_t karma,
	std::string playerName, float x, float y, float z, float rotX, float rotY,
	float rotZ) :
	m_level(level), m_hp(hp), m_maxHp(maxHP), m_ap(ap), m_maxAP(maxAP),
	m_xp(xp), m_maxXP(maxXP), m_head(head), m_torso(torso), m_leftArm(leftArm),
	m_rightArm(rightArm), m_leftLeg(leftLeg), m_rightLeg(rightLeg), m_radResist(radResist),
	m_rads(rads), m_maxRads(maxRads), m_h2o(h2o), m_maxH2o(maxH2o), m_fod(fod),
	m_maxFod(maxFod), m_slp(slp), m_maxSlp(maxSlp), m_weight(weight),
	m_maxWeight(maxWeight), m_dr(dr), m_dt(dt), m_caps(caps), m_karma(karma), 
	m_playerName(playerName), m_x(x), m_y(y), m_z(z), m_rotX(rotX), m_rotY(rotY),
	m_rotZ(rotZ)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetPlayerInfoPacket::~SetPlayerInfoPacket()
{

}

MessageType SetPlayerInfoPacket::getType()
{
	return MessageType::SETPLAYERINFO;
}

size_t SetPlayerInfoPacket::getSize()
{
	return DataPacketHeader::getHeaderSize() + 61 + m_playerName.length() + 24;
}

void SetPlayerInfoPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	*((int16_t *) (buffer + 0)) = htons(m_level);
	*((int16_t *) (buffer + 2)) = htons(m_hp);
	*((int16_t *) (buffer + 4)) = htons(m_maxHp);
	*((int16_t *) (buffer + 6)) = htons(m_ap);
	*((int16_t *) (buffer + 8)) = htons(m_maxAP);
	*((int32_t *) (buffer + 10)) = htonl(m_xp);
	*((int32_t *) (buffer + 14)) = htonl(m_maxXP);
	buffer[18] = m_head;
	buffer[19] = m_torso;
	buffer[20] = m_leftArm;
	buffer[21] = m_rightArm;
	buffer[22] = m_leftLeg;
	buffer[23] = m_rightLeg;
	buffer[24] = m_radResist;
	*((int16_t *) (buffer + 25)) = htons(m_rads);
	*((int16_t *) (buffer + 27)) = htons(m_maxRads);
	*((int16_t *) (buffer + 29)) = htons(m_h2o);
	*((int16_t *) (buffer + 31)) = htons(m_maxH2o);
	*((int16_t *) (buffer + 33)) = htons(m_fod);
	*((int16_t *) (buffer + 35)) = htons(m_maxFod);
	*((int16_t *) (buffer + 37)) = htons(m_slp);
	*((int16_t *) (buffer + 39)) = htons(m_maxSlp);
	*((int16_t *) (buffer + 41)) = htons(m_weight);
	*((int16_t *) (buffer + 43)) = htons(m_maxWeight);
	*((uint32_t *) (buffer + 45)) = htonf(m_dr);
	*((uint32_t *) (buffer + 49)) = htonf(m_dt);
	*((int32_t *)(buffer + 53)) = htonl(m_caps);
	*((int16_t *)(buffer + 57)) = htons(m_karma);
	*((int16_t *) (buffer + 59)) = htons((uint16_t)m_playerName.length());
	memcpy(buffer + 61, m_playerName.c_str(), m_playerName.length());
	buffer += 61 + m_playerName.length();
	bufferSize -= 61 + m_playerName.length();
	*((uint32_t *)(buffer + 0)) = htonf(m_x);
	*((uint32_t *)(buffer + 4)) = htonf(m_y);
	*((uint32_t *)(buffer + 8)) = htonf(m_z);
	*((uint32_t *)(buffer + 12)) = htonf(m_rotX);
	*((uint32_t *)(buffer + 16)) = htonf(m_rotY);
	*((uint32_t *)(buffer + 20)) = htonf(m_rotZ);
	
}

void SetPlayerInfoPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();

	m_level = ntohs(*((int16_t *) (buffer + 0)));
	m_hp = ntohs(*((int16_t *) (buffer + 2)));
	m_maxHp= ntohs(*((int16_t *) (buffer + 4)));
	m_ap= ntohs(*((int16_t *) (buffer + 6)));
	m_maxAP= ntohs(*((int16_t *) (buffer + 8)));
	m_xp = ntohl(*((int32_t *) (buffer + 10)));
	m_maxXP = ntohl(*((int32_t *) (buffer + 14)));
	m_head = buffer[18];
	m_torso = buffer[19];
	m_leftArm = buffer[20];
	m_rightArm = buffer[21];
	m_leftLeg = buffer[22];
	m_rightLeg = buffer[23];
	m_radResist = buffer[24];
	m_rads = ntohs(*((int16_t *) (buffer + 25)));
	m_maxRads = ntohs(*((int16_t *) (buffer + 27)));
	m_h2o = ntohs(*((int16_t *) (buffer + 29)));
	m_maxH2o = ntohs(*((int16_t *) (buffer + 31)));
	m_fod = ntohs(*((int16_t *) (buffer + 33)));
	m_maxFod = ntohs(*((int16_t *) (buffer + 35)));
	m_slp = ntohs(*((int16_t *) (buffer + 37)));
	m_maxSlp = ntohs(*((int16_t *) (buffer + 39)));
	m_weight = ntohs(*((int16_t *) (buffer + 41)));
	m_maxWeight = ntohs(*((int16_t *) (buffer + 43)));
	m_dr = ntohf(*((uint32_t *) (buffer + 45)));
	m_dt = ntohf(*((uint32_t *) (buffer + 49)));
	m_caps = ntohl(*((int32_t *)(buffer + 53)));
	m_karma = ntohs(*((int16_t *)(buffer + 57)));
	short len = ntohs(*((int16_t *) (buffer + 59)));
	m_playerName = std::string(buffer + 59, len);
	buffer += 61 + m_playerName.length();
	buffer -= 61 + m_playerName.length();
	m_x = ntohf(*((uint32_t *)(buffer + 0)));
	m_y = ntohf(*((uint32_t *)(buffer + 4)));
	m_z = ntohf(*((uint32_t *)(buffer + 8)));
	m_rotX = ntohf(*((uint32_t *)(buffer + 12)));
	m_rotY = ntohf(*((uint32_t *)(buffer + 16)));
	m_rotZ = ntohf(*((uint32_t *)(buffer + 20)));
}
