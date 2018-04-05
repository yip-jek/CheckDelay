#include "interfacefilelist.h"

InterfaceFileList::InterfaceFileList(int path_size, base::SimpleTime& period_time)
:m_pathSize(path_size)
,m_periodTime(period_time)
{
}

InterfaceFileList::~InterfaceFileList()
{
}

void InterfaceFileList::Import(const std::string& interface_file) throw(base::Exception)
{
}

int InterfaceFileList::GetPathSize() const
{
	return m_pathSize;
}

std::string InterfaceFileList::GetPeriodDay() const
{
	return m_periodTime.DayTime8();
}

