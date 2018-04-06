#include "period.h"
#include "errorcode.h"
#include "pubstr.h"
#include "pubtime.h"

Period::Period()
{
}

Period::~Period()
{
}

void Period::Init(const std::string& fmt) throw(base::Exception)
{
	if ( IsYMD(fmt) )
	{
		InitYMD(fmt);
	}
	else
	{
		InitT(fmt);
	}
}

std::string Period::GetDay() const
{
	return m_stDay.DayTime8();
}

std::string Period::GetDay_fmt(const std::string& fmt) const
{
	const std::string FMT_UP = base::PubStr::TrimUpperB(fmt);
	if ( "YYYYMMDD" == FMT_UP )
	{
		return m_stDay.DayTime8();
	}
	else if ( "YYYYMM" == FMT_UP )
	{
		return m_stDay.MonTime6();
	}
	else if ( "YYMMDD" == FMT_UP )
	{
		return m_stDay.DayTime8().substr(2);
	}
	else if ( "YYMM" == FMT_UP )
	{
		return m_stDay.MonTime6().substr(2);
	}
	else if ( "MMDD" == FMT_UP )
	{
		return m_stDay.DayTime8().substr(4);
	}
	else
	{
		return "";
	}
}

base::SimpleTime Period::GetDay_ST() const
{
	return m_stDay;
}

bool Period::IsYMD(const std::string& day_time)
{
	int i = 0;
	return (S_DAY_SIZE == day_time.size() && base::PubStr::Str2Int(day_time, i));
}

void Period::InitYMD(const std::string& day_time) throw(base::Exception)
{
	// 格式：YYYYMMDD
	int year = 0;
	if ( !base::PubStr::Str2Int(day_time.substr(0, 4), year) )
	{
		throw base::Exception(ERR_CHKDLY_PERIOD_FAIL, "账期初始化失败，年份无效：[%s] [FILE:%s, LINE:%d]", day_time.substr(0, 4).c_str(), __FILE__, __LINE__);
	}

	int mon = 0;
	if ( !base::PubStr::Str2Int(day_time.substr(4, 2), mon) )
	{
		throw base::Exception(ERR_CHKDLY_PERIOD_FAIL, "账期初始化失败，月份无效：[%s] [FILE:%s, LINE:%d]", day_time.substr(4, 2).c_str(), __FILE__, __LINE__);
	}

	int day = 0;
	if ( !base::PubStr::Str2Int(day_time.substr(6, 2), day) )
	{
		throw base::Exception(ERR_CHKDLY_PERIOD_FAIL, "账期初始化失败，日份无效：[%s] [FILE:%s, LINE:%d]", day_time.substr(6, 2).c_str(), __FILE__, __LINE__);
	}

	if ( !m_stDay.Set(year, mon, day, 0, 0, 0) )
	{
		throw base::Exception(ERR_CHKDLY_PERIOD_FAIL, "账期初始化失败，无效日期：[%s] [FILE:%s, LINE:%d]", day_time.c_str(), __FILE__, __LINE__);
	}
}

void Period::InitT(const std::string& t) throw(base::Exception)
{
	// 格式：T - n
	std::vector<std::string> vec_str;
	base::PubStr::Str2StrVector(t, "-", vec_str);

	if ( vec_str.size() != 2 )
	{
		throw base::Exception(ERR_CHKDLY_PERIOD_FAIL, "账期初始化失败，格式非法：[%s] [FILE:%s, LINE:%d]", t.c_str(), __FILE__, __LINE__);
	}

	if ( base::PubStr::UpperB(vec_str[0]) != "T" )
	{
		throw base::Exception(ERR_CHKDLY_PERIOD_FAIL, "账期初始化失败，格式非法：[%s] [FILE:%s, LINE:%d]", t.c_str(), __FILE__, __LINE__);
	}

	unsigned int n = 0;
	if ( !base::PubStr::Str2UInt(vec_str[1], n) )
	{
		throw base::Exception(ERR_CHKDLY_PERIOD_FAIL, "账期初始化失败，格式非法：[%s] [FILE:%s, LINE:%d]", t.c_str(), __FILE__, __LINE__);
	}

	long long ll_day = 0;
	std::string day = base::PubTime::DateNowMinusDays(n) + "000000";
	base::PubStr::Str2LLong(day, ll_day);

	if ( !m_stDay.Set(ll_day, true) )
	{
		throw base::Exception(ERR_CHKDLY_PERIOD_FAIL, "账期初始化失败，格式非法：[%s] [FILE:%s, LINE:%d]", t.c_str(), __FILE__, __LINE__);
	}
}

