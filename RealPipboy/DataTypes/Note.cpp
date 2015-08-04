#include "Note.h"

Note::Note(const std::string &title, unsigned char type, const std::string &content, unsigned char flags) :
	m_title(title), m_type(type), m_content(content), m_flags(flags)
{

}

Note::~Note()
{
}

const std::string & Note::getTitle()
{
	return m_title;
}

unsigned char Note::getType()
{
	return m_type;
}

const std::string & Note::getContent()
{
	return m_content;
}

unsigned char Note::getFlags() {
	return m_flags;
}

void Note::setTitle(const std::string &title)
{
	m_title = title;
}

void Note::setContent(const std::string &content)
{
	m_content = content;
}
