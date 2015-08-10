#include "Radio.h"


Radio::Radio(uint32_t id, const std::string &name, bool active, bool playing) :
	m_id(id), m_name(name), m_flags(0)
{
	if (active) m_flags |= RADIO_FLAG_ACTIVE;
	if (playing) m_flags |= RADIO_FLAG_PLAYING;
}

Radio::Radio(uint32_t id, const std::string &name, uint8_t flags) :
	m_id(id), m_name(name), m_flags(flags)
{

}

Radio::~Radio()
{
}

uint32_t Radio::getId() const
{
	return m_id;
}

const std::string & Radio::getName() const
{
	return m_name;
}

uint8_t Radio::getFlags() const
{
	return m_flags;
}

bool Radio::isActive() const
{
	return (m_flags & RADIO_FLAG_ACTIVE) != 0;
}

bool Radio::isPlaying() const
{
	return (m_flags & RADIO_FLAG_PLAYING) != 0;
}
