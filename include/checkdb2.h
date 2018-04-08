#pragma once

#include <vector>
#include "basedb2.h"

struct OutputFileState;

class CheckDB2 : public base::BaseDB2
{
public:
	CheckDB2(const std::string& db_name, const std::string& usr, const std::string& pw);
	virtual ~CheckDB2();

public:
	// 设置输出表名
	void SetOutputTab(const std::string& output_tab);

	// 删除旧账期数据
	void DeleteOldData(const std::string& date_time) throw(base::Exception);

	// 插入新数据
	void InsertResult(const std::vector<OutputFileState>& vec_ofs) throw(base::Exception);

private:
	// 是否无数据被删除
	bool IsNoRowDeleted(const std::string& exception);

private:
	std::string m_outputTab;		// 输出表名
};

