#pragma once

#include <string>

class PlayerEffect
{
public:
	PlayerEffect(const std::string &name, const std::string &effects);
	~PlayerEffect();

	const std::string &getName();
	const std::string &getEffects();
	void setName(const std::string &name);
	void setEffects(const std::string &effects);

private:
	std::string m_name;
	std::string m_effects;
};

