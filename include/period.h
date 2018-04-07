#pragma once

#include "exception.h"
#include "simpletime.h"

// 账期
class Period
{
public:
	Period();
	~Period();

	static const unsigned int S_DAY_SIZE = 8;

	static bool IsDayType(const std::string fmt);
	static bool IsMonType(const std::string fmt);

public:
	void Init(const std::string& fmt) throw(base::Exception);

	// 返回账期天，格式：YYYYMMDD
	std::string GetDay() const;

	// 返回账期天，指定格式
	// 支持的格式：
	// 1) YYYYMMDD, 例如 20180401
	// 2) YYYYMM, 例如 201804
	// 3) YYMMDD, 例如 180401
	// 4) YYMM, 例如 1804
	// 5) MMDD, 例如 0401
	std::string GetDay_fmt(const std::string& fmt) const;

	// 返回账期时间
	base::SimpleTime GetDay_ST() const;

private:
	// 是否为8位时间（YYYYMMDD）
	bool IsYMD(const std::string& day_time);

	void InitYMD(const std::string& day_time) throw(base::Exception);

	void InitT(const std::string& t) throw(base::Exception);

private:
	base::SimpleTime m_stDay;
};

