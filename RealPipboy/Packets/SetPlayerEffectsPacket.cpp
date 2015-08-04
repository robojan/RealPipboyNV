#include "SetPlayerEffectsPacket.h"

#include <cassert>
#include <cstdint>
#include <winsock2.h>

SetPlayerEffectsPacket::SetPlayerEffectsPacket(const char *buffer, size_t bufferSize) :
DataPacket(DataPacketHeader(buffer, bufferSize))
{
	assert(m_header.getDataSize() + DataPacketHeader::getHeaderSize() <= bufferSize);

	readPacket(buffer, bufferSize);
}

SetPlayerEffectsPacket::SetPlayerEffectsPacket(const std::vector<StatusEffect> &rad,
											   const std::vector<StatusEffect> &h2o,
											   const std::vector<StatusEffect> &fod,
											   const std::vector<StatusEffect> &slp,
											   const std::vector<PlayerEffect> &effects) :
											   m_radEffects(rad), m_h2oEffects(h2o),
											   m_fodEffects(fod), m_slpEffects(slp),
											   m_playerEffects(effects)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetPlayerEffectsPacket::SetPlayerEffectsPacket(const std::vector<StatusEffect> &rad, 
											   const std::vector<PlayerEffect> &effects) :
											   m_radEffects(rad), m_playerEffects(effects)
{
	m_header = DataPacketHeader(getType(), getSize() - DataPacketHeader::getHeaderSize());
	m_valid = true;
}

SetPlayerEffectsPacket::~SetPlayerEffectsPacket()
{

}

MessageType SetPlayerEffectsPacket::getType()
{
	return MessageType::SETPLAYEREFFECTS;
}

size_t SetPlayerEffectsPacket::getSize()
{
	int dataSize = 6;
	for (std::vector<StatusEffect>::iterator it = m_radEffects.begin(); it != m_radEffects.end();
		 ++it) {
		dataSize += 3 + it->getAbbrev().length();
	}
	for (std::vector<StatusEffect>::iterator it = m_h2oEffects.begin(); it != m_h2oEffects.end();
		 ++it) {
		dataSize += 3 + it->getAbbrev().length();
	}
	for (std::vector<StatusEffect>::iterator it = m_fodEffects.begin(); it != m_fodEffects.end();
		 ++it) {
		dataSize += 3 + it->getAbbrev().length();
	}
	for (std::vector<StatusEffect>::iterator it = m_slpEffects.begin(); it != m_slpEffects.end();
		 ++it) {
		dataSize += 3 + it->getAbbrev().length();
	}
	for (std::vector<StatusEffect>::iterator it = m_radEffects.begin(); it != m_radEffects.end();
		 ++it) {
		dataSize += 3 + it->getAbbrev().length();
	}
	for (std::vector<PlayerEffect>::iterator it = m_playerEffects.begin(); 
		it != m_playerEffects.end(); ++it) {
		dataSize += 4 + it->getEffects().length() + it->getName().length();
	}

	return DataPacketHeader::getHeaderSize() + dataSize;
}

void SetPlayerEffectsPacket::fillBuffer(char *buffer, size_t bufferSize)
{
	assert(bufferSize >= getSize());
	size_t offset = m_header.fillBuffer(buffer, bufferSize);
	buffer += offset;
	bufferSize -= offset;
	// Rad effects
	buffer[0] = (unsigned char) m_radEffects.size();
	buffer++;
	for (std::vector<StatusEffect>::iterator it = m_radEffects.begin(); it != m_radEffects.end();
		 ++it) {
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getAbbrev().length());
		memcpy(buffer + 2, it->getAbbrev().c_str(), it->getAbbrev().length());
		buffer += 2 + it->getAbbrev().length();
		buffer[0] = it->getEffect();
		buffer++;
	}
	// H2O effects
	buffer[0] = (unsigned char) m_h2oEffects.size();
	buffer++;
	for (std::vector<StatusEffect>::iterator it = m_h2oEffects.begin(); it != m_h2oEffects.end();
		 ++it) {
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getAbbrev().length());
		memcpy(buffer + 2, it->getAbbrev().c_str(), it->getAbbrev().length());
		buffer += 2 + it->getAbbrev().length();
		buffer[0] = it->getEffect();
		buffer++;
	}
	// Rad effects
	buffer[0] = (unsigned char) m_fodEffects.size();
	buffer++;
	for (std::vector<StatusEffect>::iterator it = m_fodEffects.begin(); it != m_fodEffects.end();
		 ++it) {
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getAbbrev().length());
		memcpy(buffer + 2, it->getAbbrev().c_str(), it->getAbbrev().length());
		buffer += 2 + it->getAbbrev().length();
		buffer[0] = it->getEffect();
		buffer++;
	}
	// Rad effects
	buffer[0] = (unsigned char) m_slpEffects.size();
	buffer++;
	for (std::vector<StatusEffect>::iterator it = m_slpEffects.begin(); it != m_slpEffects.end();
		 ++it) {
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getAbbrev().length());
		memcpy(buffer + 2, it->getAbbrev().c_str(), it->getAbbrev().length());
		buffer += 2 + it->getAbbrev().length();
		buffer[0] = it->getEffect();
		buffer++;
	}
	// Player effects
	*((int16_t *) (buffer + 0)) = htons((uint16_t)m_playerEffects.size());
	buffer += 2;
	for (std::vector<PlayerEffect>::iterator it = m_playerEffects.begin(); 
		 it != m_playerEffects.end(); ++it) {
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getName().length());
		memcpy(buffer + 2, it->getName().c_str(), it->getName().length());
		buffer += 2 + it->getName().length();
		*((int16_t *) (buffer + 0)) = htons((uint16_t)it->getEffects().length());
		memcpy(buffer + 2, it->getEffects().c_str(), it->getEffects().length());
		buffer += 2 + it->getEffects().length();
	}
}

void SetPlayerEffectsPacket::readPacket(const char *buffer, size_t bufferSize)
{
	assert(containsCompletePacket(buffer, bufferSize));
	buffer += DataPacketHeader::getHeaderSize();
	bufferSize -= DataPacketHeader::getHeaderSize();

	signed char numRadEffects = buffer[0];
	buffer++;
	bufferSize--;
	for (int i = 0; i < numRadEffects && bufferSize >= 2; i++) {
		int strLen = ntohs(*((int16_t *) (buffer + 0)));
		std::string name(buffer + 2, strLen);
		m_radEffects.push_back(StatusEffect(name, buffer[2 + strLen]));
		buffer += 3 + strLen;
		bufferSize -= 3 + strLen;
	}
	signed char numH2oEffects = buffer[0];
	buffer++;
	bufferSize--;
	for (int i = 0; i < numH2oEffects && bufferSize >= 2; i++) {
		int strLen = ntohs(*((int16_t *) (buffer + 0)));
		std::string name(buffer + 2, strLen);
		m_h2oEffects.push_back(StatusEffect(name, buffer[2 + strLen]));
		buffer += 3 + strLen;
		bufferSize -= 3 + strLen;
	}
	signed char numFodEffects = buffer[0];
	buffer++;
	bufferSize--;
	for (int i = 0; i < numFodEffects && bufferSize >= 2; i++) {
		int strLen = ntohs(*((int16_t *) (buffer + 0)));
		std::string name(buffer + 2, strLen);
		m_fodEffects.push_back(StatusEffect(name, buffer[2 + strLen]));
		buffer += 3 + strLen;
		bufferSize -= 3 + strLen;
	}
	signed char numSlpEffects = buffer[0];
	buffer++;
	bufferSize--;
	for (int i = 0; i < numSlpEffects && bufferSize >= 2; i++) {
		int strLen = ntohs(*((int16_t *) (buffer + 0)));
		std::string name(buffer + 2, strLen);
		m_slpEffects.push_back(StatusEffect(name, buffer[2 + strLen]));
		buffer += 3 + strLen;
		bufferSize -= 3 + strLen;
	}
	short playerEffects = ntohs(*((int16_t *) (buffer + 0)));
	buffer += 2;
	bufferSize -= 2;
	for (int i = 0; i < playerEffects && bufferSize >= 4; i++) {
		int nameLen = ntohs(*((int16_t *) (buffer + 0)));
		std::string name(buffer + 2, nameLen);
		int effectLen = ntohs(*((int16_t *) (buffer + 2 + nameLen)));
		std::string effect(buffer + 4 + nameLen, effectLen);
		m_playerEffects.push_back(PlayerEffect(name, effect));
		buffer += 4 + nameLen + effectLen;
		bufferSize -= 4 + nameLen + effectLen;
	}

	m_valid = true;
}

const std::vector<StatusEffect> & SetPlayerEffectsPacket::getRadEffects()
{
	return m_radEffects;
}

const std::vector<StatusEffect> & SetPlayerEffectsPacket::getH2OEffects()
{
	return m_h2oEffects;
}

const std::vector<StatusEffect> & SetPlayerEffectsPacket::getFodEffects()
{
	return m_fodEffects;
}

const std::vector<StatusEffect> & SetPlayerEffectsPacket::getSlpEffects()
{
	return m_slpEffects;
}

const std::vector<PlayerEffect> & SetPlayerEffectsPacket::getPlayerEffects()
{
	return m_playerEffects;
}

void SetPlayerEffectsPacket::setRadEffects(const std::vector<StatusEffect> &rad)
{
	m_radEffects = rad;
}

void SetPlayerEffectsPacket::setH2OEffects(const std::vector<StatusEffect> &h2o)
{
	m_h2oEffects = h2o;
}

void SetPlayerEffectsPacket::setFodEffects(const std::vector<StatusEffect> &fod)
{
	m_fodEffects = fod;
}

void SetPlayerEffectsPacket::setSlpEffects(const std::vector<StatusEffect> &slp)
{
	m_slpEffects = slp;
}

void SetPlayerEffectsPacket::setPlayerEffects(const std::vector<PlayerEffect> &effects)
{
	m_playerEffects = effects;
}
