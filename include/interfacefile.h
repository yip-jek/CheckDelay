#pragma once

#include <string>

class InterfaceFile
{
public:
	InterfaceFile();
	~InterfaceFile();

private:
	int          m_pathID;
	std::string  m_channel;
	std::string  m_filename;
	int          m_days;
	int          m_hour;
	unsigned int m_filesize;

private:
};

