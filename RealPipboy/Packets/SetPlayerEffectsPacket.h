#pragma once

#include "DataPacket.h"
#include "MessageTypes.h"

#include "../DataTypes/PlayerEffect.h"
#include "../DataTypes/StatusEffect.h"

#include <string>
#include <vector>

// Packet format
// startIdx	size	type		description
// 0		2		int8[2]		Sync word 'R''P'
// 2		1		int8		Type of the packet
// 3		2		uint16		Size of the packet
// 5		1		int8		Number of Rad Effects
// 6		2		int16		length Rad effect name 
// 8		n		char[]		Rad effect name
// 8+n		1		int8		Effect value
// repeat
// ..		1		int8		Number of h2o Effects
// ..		2		int16		length h2o effect name 
// ..		n		char[]		h2o effect name
// ..		1		int8		Effect value
// repeat
// ..		1		int8		Number of fod Effects
// ..		2		int16		length fod effect name 
// ..		n		char[]		fod effect name
// ..		1		int8		Effect value
// repeat
// ..		1		int8		Number of slp Effects
// ..		2		int16		length slp effect name 
// ..		n		char[]		slp effect name
// ..		1		int8		Effect value
// repeat
// ..		2		int16		number of PlayerEffects
// ..		2		int16		nameLength
// ..		n		char[]		name
// ..		2		int16		effects length
// ..		n		char[]		effects
// repeat


class SetPlayerEffectsPacket :
	public DataPacket
{
public:
	SetPlayerEffectsPacket(const char *buffer, size_t bufferSize);
	SetPlayerEffectsPacket(const std::vector<StatusEffect> &rad, 
						   const std::vector<StatusEffect> &h2o,
						   const std::vector<StatusEffect> &fod,
						   const std::vector<StatusEffect> &slp,
						   const std::vector<PlayerEffect> &effects);
	SetPlayerEffectsPacket(const std::vector<StatusEffect> &rad,
						   const std::vector<PlayerEffect> &effects);
	virtual ~SetPlayerEffectsPacket();

	virtual MessageType getType();
	virtual size_t getSize();

	virtual void fillBuffer(char *buffer, size_t bufferSize);

	const std::vector<StatusEffect> &getRadEffects();
	const std::vector<StatusEffect> &getH2OEffects();
	const std::vector<StatusEffect> &getFodEffects();
	const std::vector<StatusEffect> &getSlpEffects();
	const std::vector<PlayerEffect> &getPlayerEffects();

	void setRadEffects(const std::vector<StatusEffect> &rad);
	void setH2OEffects(const std::vector<StatusEffect> &h2o);
	void setFodEffects(const std::vector<StatusEffect> &fod);
	void setSlpEffects(const std::vector<StatusEffect> &slp);
	void setPlayerEffects(const std::vector<PlayerEffect> &effects);

private:
	void readPacket(const char *buffer, size_t bufferSize);

	std::vector<StatusEffect> m_radEffects;
	std::vector<StatusEffect> m_h2oEffects;
	std::vector<StatusEffect> m_fodEffects;
	std::vector<StatusEffect> m_slpEffects;
	std::vector<PlayerEffect> m_playerEffects;
};

