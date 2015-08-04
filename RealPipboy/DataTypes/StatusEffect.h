#pragma once

#include <string>

class StatusEffect
{
public:
	StatusEffect(const std::string &abbrev, signed char effect);
	~StatusEffect();

	const std::string &getAbbrev();
	signed char getEffect();

	void setAbbrev(const std::string &abbrev);
	void setEffect(signed char  effect);

private:
	std::string m_abbrev;
	signed char m_effect;
};

