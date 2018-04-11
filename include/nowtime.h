#pragma once

#include "simpletime.h"

class NowTime
{
public:
	NowTime();
	~NowTime();

	static const char* const S_NT_NOW;
	static const char* const S_NT_YEAR;
	static const char* const S_NT_MON;
	static const char* const S_NT_DAY;
	static const char* const S_NT_HOUR;
	static const char* const S_NT_MIN;
	static const char* const S_NT_SEC;

public:
	bool Init(const std::string& fmt, std::string& now_time);

private:
	std::string Convert(const std::string& fmt);

private:
	base::SimpleTime m_stNow;
};

