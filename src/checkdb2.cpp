#include "checkdb2.h"

CheckDB2::CheckDB2(const std::string& db_name, const std::string& usr, const std::string& pw)
:BaseDB2(db_name, usr, pw)
{
}

CheckDB2::~CheckDB2()
{
}

void CheckDB2::SetOutputTab(const std::string& output_tab)
{
}

void CheckDB2::DeleteOldData(const std::string& date_time) throw(base::Exception)
{
}

void CheckDB2::InsertResult(const std::vector<OutputFileState>& vec_ofs) throw(base::Exception)
{
}

