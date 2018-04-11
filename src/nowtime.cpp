#include "nowtime.h"
#include "pubstr.h"

const char* const NowTime::S_NT_NOW  = "NOW";
const char* const NowTime::S_NT_YEAR = "YYYY";
const char* const NowTime::S_NT_MON  = "MM";
const char* const NowTime::S_NT_DAY  = "DD";
const char* const NowTime::S_NT_HOUR = "HH";
const char* const NowTime::S_NT_MIN  = "MI";
const char* const NowTime::S_NT_SEC  = "SS";

NowTime::NowTime()
:m_stNow(base::SimpleTime::Now())
{
}

NowTime::~NowTime()
{
}

bool NowTime::Init(const std::string& fmt, std::string& now_time)
{
	std::string upper_fmt = base::PubStr::UpperB(fmt);

	std::vector<std::string> vec_str;
	base::PubStr::Str2StrVector(upper_fmt, ":", vec_str);

	if ( vec_str.size() <= 1U )
	{
		return false;
	}

	upper_fmt.erase(0, upper_fmt.find(':')+1);
	base::PubStr::Trim(upper_fmt);

	now_time = Convert(upper_fmt);
	return true;
}

std::string NowTime::Convert(const std::string& fmt)
{
	const std::string ST_NOW = m_stNow.Time14();
	std::string nt = fmt;

	size_t pos = nt.find(S_NT_YEAR);
	if ( pos != std::string::npos )
	{
		nt.replace(pos, 4, ST_NOW.substr(0, 4));
	}

	pos = nt.find(S_NT_MON);
	if ( pos != std::string::npos )
	{
		nt.replace(pos, 2, ST_NOW.substr(4, 2));
	}

	pos = nt.find(S_NT_DAY);
	if ( pos != std::string::npos )
	{
		nt.replace(pos, 2, ST_NOW.substr(6, 2));
	}

	pos = nt.find(S_NT_HOUR);
	if ( pos != std::string::npos )
	{
		nt.replace(pos, 2, ST_NOW.substr(8, 2));
	}

	pos = nt.find(S_NT_MIN);
	if ( pos != std::string::npos )
	{
		nt.replace(pos, 2, ST_NOW.substr(10, 2));
	}

	pos = nt.find(S_NT_SEC);
	if ( pos != std::string::npos )
	{
		nt.replace(pos, 2, ST_NOW.substr(12, 2));
	}

	return nt;
}

