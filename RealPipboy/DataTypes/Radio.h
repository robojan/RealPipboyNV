#pragma once

#include <cstdint>
#include <string>

#define RADIO_FLAG_ACTIVE		0x1
#define RADIO_FLAG_PLAYING		0x2

class Radio
{
public:
	Radio(uint32_t id, const std::string &name, bool active, bool playing);
	Radio(uint32_t id, const std::string &name, uint8_t flags);
	~Radio();

	uint32_t getId() const;
	const std::string &getName() const;
	uint8_t getFlags() const;
	bool isActive() const;
	bool isPlaying() const;

private:
	uint32_t m_id;
	std::string m_name;
	uint8_t m_flags;
};

