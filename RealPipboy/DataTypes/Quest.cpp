#include "Quest.h"


QuestObjective::QuestObjective(unsigned int objectID, const std::string &text, 
	unsigned int flags) : m_objectiveID(objectID), m_text(text), m_flags(flags)
{

}

QuestObjective::~QuestObjective()
{

}

void QuestObjective::setText(const std::string &text)
{
	m_text = text;
}

const std::string & QuestObjective::getText() const
{
	return m_text;
}

unsigned int QuestObjective::getFlags() const
{
	return m_flags;
}

unsigned int QuestObjective::getObjectiveID() const
{
	return m_objectiveID;
}

bool QuestObjective::isCompleted() const
{
	return (m_flags & QUESTFLAG_COMPLETED) != 0;
}

void QuestObjective::setCompleted(bool completed)
{
	if (completed) {
		m_flags |= QUESTFLAG_COMPLETED;
	}
	else {
		m_flags &= ~QUESTFLAG_COMPLETED;
	}
}

bool QuestObjective::isDisplayed() const
{
	return (m_flags & QUESTFLAG_DISPLAYED) != 0;
}

void QuestObjective::setDisplayed(bool displayed)
{
	if (displayed) {
		m_flags |= QUESTFLAG_DISPLAYED;
	}
	else {
		m_flags &= ~QUESTFLAG_DISPLAYED;
	}
}

Quest::Quest(unsigned int id, const std::string &name, unsigned int flags) :
	m_name(name), m_flags(flags), m_questID(id)
{

}

Quest::~Quest()
{

}

void Quest::addObjective(QuestObjective &objective)
{
	m_objectives.push_back(objective);
}

void Quest::setName(const std::string &name)
{
	m_name = name;
}

const std::string & Quest::getName() const
{
	return m_name;
}

const std::list<QuestObjective> & Quest::getObjectives() const
{
	return m_objectives;
}

unsigned int Quest::getFlags() const
{
	return m_flags;
}

void Quest::setCompleted(bool completed)
{
	if (completed) {
		m_flags |= QUESTFLAG_COMPLETED;
	}
	else {
		m_flags &= ~QUESTFLAG_COMPLETED;
	}
}

bool Quest::isCompleted() const
{
	return (m_flags & QUESTFLAG_COMPLETED) != 0;
}

bool Quest::isActive() const
{
	return (m_flags & QUESTFLAG_COMPLETED) != 0;
}

void Quest::setActive(bool active)
{
	if (active) {
		m_flags |= QUESTFLAG_ACTIVEQUEST;
	}
	else {
		m_flags &= ~QUESTFLAG_ACTIVEQUEST;
	}
}

unsigned int Quest::getQuestID() const
{
	return m_questID;
}

void Quest::setQuestID(unsigned int id)
{
	m_questID = id;
}
