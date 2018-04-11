#pragma once

#include <map>
#include <vector>
#include "exception.h"
#include "simpletime.h"

namespace base
{
class Log;
}

struct FDFileInfo;
class IFFileState;
class InterfaceFileList;
class Period;
struct OutputFileState;

class InterfaceFile
{
public:
	typedef std::map<std::string, IFFileState>			MAP_IFFILE_STATE;

public:
	InterfaceFile(InterfaceFileList& if_file_list, const Period& period);
	InterfaceFile(const InterfaceFile& iff);
	~InterfaceFile();

	InterfaceFile& operator = (const InterfaceFile& iff);

	static const unsigned int S_INTERFACE_FILE_FMT_SIZE = 6;		// 文件规则格式大小
	static const char* const  S_CHANNEL_TAG;

public:
	void Init(const std::string& fmt) throw(base::Exception);

	void UpdateFileState(const FDFileInfo& f_info);

	void ExportOutputFile(OutputFileState& ofs, std::vector<OutputFileState>& vec_ofs);

	int          GetPathSeq()       const { return m_pathSeq      ; }
	std::string  GetChannel()       const { return m_channel      ; }
	std::string  GetFileName()      const { return m_fileName     ; }
	int          GetDays()          const { return m_days         ; }
	int          GetHour()          const { return m_hour         ; }
	unsigned int GetFileBlankSize() const { return m_fileBlanksize; }

private:
	void Explain(const std::string& fmt) throw(base::Exception);

	void Check() throw(base::Exception);

	// 是否为月账期
	void CheckMonPeriod();

	void Expand() throw(base::Exception);

	void ExpandEstimatedTime() throw(base::Exception);

	void ExpandFileNameSet() throw(base::Exception);

	bool IsEstimatedTimeLater();

	std::string ExpandFileNamePeriod(const Period* p) throw(base::Exception);

	void ExpandFileNameOptions(const std::string& file_name);

	bool ExpandOneOption(const std::string& file_name, std::vector<std::string>& vec_fn);

private:
	base::Log*         m_pLog;
	InterfaceFileList* m_pIFFileList;
	const Period*      m_pPeriod;
	base::SimpleTime   m_stNow;					// 当前系统时间

private:
	int                m_pathSeq;				// 目录序号
	std::string        m_channel;				// 渠道
	std::string        m_fileName;				// 文件名
	int                m_days;					// 延迟天数
	int                m_hour;					// 预计到达时间点：小时
	unsigned int       m_fileBlanksize;			// 文件为空的大小

private:
	bool               m_isMonPeriod;			// 是否为月账期
	MAP_IFFILE_STATE   m_mapFileNameEx;			// (扩展) 文件名集
	base::SimpleTime   m_estimatedTime;			// 预计文件到达时间
};

