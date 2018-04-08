#pragma once

#include "basedb2.h"

class CheckDB2 : public BaseDB2
{
public:
	CheckDB2(const std::string& db_name, const std::string& usr, const std::string& pw);
	virtual ~CheckDB2();

public:
	void SetOutputTab(const std::string& output_tab);

	void DeleteOldData(const std::string& date_time) throw(base::Exception);

	void InsertResult(const std::vector<OutputFileState>& vec_ofs) throw(base::Exception);

private:
	std::string m_outputTab;
};

