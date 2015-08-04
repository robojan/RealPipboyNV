#pragma once

#include <string>

#define NOTETYPE_SOUND 0
#define NOTETYPE_TEXT 1
#define NOTETYPE_IMAGE 2
#define NOTETYPE_VOICE 3

#define NOTEFLAG_READ 1

class Note
{
public:
	Note(const std::string &title, unsigned char type, const std::string &content, unsigned char flags);
	~Note();

	const std::string &getTitle();
	unsigned char getType();
	const std::string &getContent();
	unsigned char getFlags();

	void setTitle(const std::string &title);
	void setContent(const std::string &content);

private:
	std::string m_title;
	unsigned char m_type;
	std::string m_content;
	unsigned char m_flags;
};

