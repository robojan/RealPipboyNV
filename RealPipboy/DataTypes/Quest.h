#pragma once

#include <string>
#include <list>

#define QUESTFLAG_COMPLETED 1
#define QUESTFLAG_ACTIVEQUEST 2
#define QUESTFLAG_DISPLAYED 4

class QuestObjective {
public:
	struct Target {
		uint32_t id;
		float x;
		float y;
		float z;
	};

	QuestObjective(unsigned int objectID, const std::string &text, unsigned int flags);
	~QuestObjective();

	void setText(const std::string &text);
	const std::string &getText() const;
	unsigned int getFlags() const;
	unsigned int getObjectiveID() const;
	bool isCompleted() const;
	void setCompleted(bool completed);
	bool isDisplayed() const;
	void setDisplayed(bool displayed);
	void addTarget(const Target &target);
	const std::list<Target> *getTargets() const;

private:
	std::string m_text;
	unsigned int m_flags;
	unsigned int m_objectiveID;
	std::list<Target> m_targets;
};

class Quest
{
public:
	Quest(unsigned int questID, const std::string &name, unsigned int flags);
	~Quest();

	void addObjective(QuestObjective &objective);
	void setName(const std::string &name);
	const std::string &getName() const;
	const std::list<QuestObjective> &getObjectives() const;
	unsigned int getFlags() const;
	void setCompleted(bool completed);
	bool isCompleted() const;
	bool isActive() const;
	void setActive(bool active);
	unsigned int getQuestID() const;
	void setQuestID(unsigned int id);
	
private:
	std::string m_name;
	std::list<QuestObjective> m_objectives;
	unsigned int m_flags;
	unsigned int m_questID;
};

