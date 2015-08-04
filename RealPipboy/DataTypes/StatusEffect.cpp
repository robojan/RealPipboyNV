#include "StatusEffect.h"

StatusEffect::StatusEffect(const std::string &abbrev, signed char effect) :
m_abbrev(abbrev), m_effect(effect)
{

}

StatusEffect::~StatusEffect()
{
}

const std::string & StatusEffect::getAbbrev()
{
	return m_abbrev;
}

signed char StatusEffect::getEffect()
{
	return m_effect;
}

void StatusEffect::setAbbrev(const std::string &abbrev)
{
	m_abbrev = abbrev;
}

void StatusEffect::setEffect(signed char effect)
{
	m_effect = effect;
}
