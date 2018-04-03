#pragma once

#include <string>
#include <vector>
#include <set>

class InterfaceFileList
{
public:
	InterfaceFileList();
	~InterfaceFileList();

private:
	int                   m_pathSize;			// 路径个数
	//std::set<std::string> m_setChannel;			// 渠道集
};

