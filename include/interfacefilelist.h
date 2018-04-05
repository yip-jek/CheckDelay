#pragma once

#include <string>
#include <vector>
#include <set>
#include "exception.h"
#include "simpletime.h"

class InterfaceFileList
{
public:
	InterfaceFileList(int path_size, base::SimpleTime& period_time);
	~InterfaceFileList();

public:
	void Import(const std::string& interface_file) throw(base::Exception);

	int GetPathSize() const;

	// YYYYMMDD
	std::string GetPeriodDay() const;

private:
	int              m_pathSize;			// 路径个数
	base::SimpleTime m_periodTime;			// 账期时间
	//std::set<std::string> m_setChannel;			// 渠道集
};

