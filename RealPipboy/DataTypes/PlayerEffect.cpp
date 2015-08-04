#include "PlayerEffect.h"


PlayerEffect::PlayerEffect(const std::string &name, const std::string &effects) :
m_name(name), m_effects(effects)
{

}

PlayerEffect::~PlayerEffect()
{
}

const std::string & PlayerEffect::getName()
{
	return m_name;
}

const std::string & PlayerEffect::getEffects()
{
	return m_effects;
}

void PlayerEffect::setName(const std::string &name)
{
	m_name = name;
}

void PlayerEffect::setEffects(const std::string &effects)
{
	m_effects = effects;
}
