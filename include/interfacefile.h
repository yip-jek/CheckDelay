#pragma once

#include <set>
#include "exception.h"
#include "simpletime.h"

namespace base
{
class Log;
}

class InterfaceFileList;
class Period;

class InterfaceFile
{
public:
	InterfaceFile(InterfaceFileList& if_file_list, const Period& period);
	InterfaceFile(const InterfaceFile& iff);
	~InterfaceFile();

	InterfaceFile& operator = (const InterfaceFile& iff);

	static const unsigned int S_INTERFACE_FILE_FMT_SIZE = 6;		// 文件规则格式大小

public:
	void Init(const std::string& fmt) throw(base::Exception);

	int          GetPathSeq()       const { return m_pathSeq      ; }
	std::string  GetChannel()       const { return m_channel      ; }
	std::string  GetFileName()      const { return m_fileName     ; }
	int          GetDays()          const { return m_days         ; }
	int          GetHour()          const { return m_hour         ; }
	unsigned int GetFileBlankSize() const { return m_fileBlanksize; }

private:
	void Explain(const std::string& fmt) throw(base::Exception);

	void Check() throw(base::Exception);

	void Expand() throw(base::Exception);

	void ExpandEstimatedTime() throw(base::Exception);

	void ExpandFileNameSet() throw(base::Exception);

private:
	base::Log*            m_pLog;
	InterfaceFileList*    m_pIFFileList;
	const Period*         m_pPeriod;

private:
	int                   m_pathSeq;				// 目录序号
	std::string           m_channel;				// 渠道
	std::string           m_fileName;				// 文件名
	int                   m_days;					// 延迟天数
	int                   m_hour;					// 预计到达时间点：小时
	unsigned int          m_fileBlanksize;			// 文件为空的大小

private:
	std::set<std::string> m_setFileNameEx;			// (扩展) 文件名集
	base::SimpleTime      m_estimatedTime;			// 预计文件到达时间
};

