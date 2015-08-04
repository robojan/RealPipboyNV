#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"
#include <string>
#include <cstdint>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		2		int16		level
// 7		2		int16		HP
// 9		2		int16		Max HP
// 11		2		int16		AP
// 13		2		int16		Max AP
// 15		4		int32		XP
// 19		4		int32		Max XP
// 23		1		int8		Head condition[0-100]
// 24		1		int8		Torso condition[0-100]
// 25		1		int8		Left arm condition[0-100]
// 26		1		int8		Right arm condition[0-100]
// 27		1		int8		Left leg condition[0-100]
// 28		1		int8		Right leg condition[0-100]
// 29		1		int8		Rad resistance[0-100]
// 30		2		int16		Rads
// 32		2		int16		Max Rads
// 34		2		int16		H2O
// 36		2		int16		Max H2O
// 38		2		int16		FOD
// 40		2		int16		Max FOD
// 42		2		int16		SLP
// 44		2		int16		Max SLP
// 46		2		int16		Player weight
// 48		2		int16		Max weight
// 50		4		float		Damage resist
// 54		4		float		Damage threshold
// 58		4		int			Caps
// 62		2		int16		karma
// 64		2		int16		Name Length
// 66		len		char[]		Player name
// ..		4		float		posX
// ..		4		float		posY
// ..		4		float		posZ
// ..		4		float		rotX
// ..		4		float		rotY
// ..		4		float		rotZ

class SetPlayerInfoPacket :
	public DataPacket
{
public:
	SetPlayerInfoPacket(const char *buffer, size_t bufferSize);
	SetPlayerInfoPacket(int16_t level, int16_t hp, int16_t maxHP, int16_t ap, int16_t maxAP, 
						int32_t xp, int32_t maxXP, int8_t head, int8_t torso, int8_t leftArm,
						int8_t rightArm, int8_t leftLeg, int8_t rightLeg, int8_t radResist,
						int16_t rads, int16_t maxRads, int16_t h2o, int16_t maxH2o, 
						int16_t fod, int16_t maxFod, int16_t slp, int16_t maxSlp, int16_t weight,
						int16_t maxWeight, float dr, float dt, int32_t caps, int16_t karma,
						std::string playerName, float x, float y, float z, float rotX, float rotY, 
						float rotZ);
	virtual ~SetPlayerInfoPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

private:
	void readPacket(const char *buffer, size_t bufferSize);

	int16_t m_level;
	int16_t m_hp, m_maxHp, m_ap, m_maxAP;
	int32_t m_xp, m_maxXP;
	int8_t m_head, m_torso, m_leftArm, m_rightArm, m_leftLeg, m_rightLeg;
	int8_t m_radResist;
	int16_t m_rads, m_maxRads, m_h2o, m_maxH2o, m_fod, m_maxFod, m_slp, m_maxSlp;
	int16_t m_weight, m_maxWeight, m_karma;
	float m_dr, m_dt;
	int32_t m_caps;
	std::string m_playerName;
	float m_x, m_y, m_z, m_rotX, m_rotY, m_rotZ;
};

