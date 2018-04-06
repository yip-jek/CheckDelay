#include "interfacefile.h"
#include "interfacefilelist.h"
#include "errorcode.h"
#include "log.h"
#include "pubtime.h"
#include "pubstr.h"
#include "period.h"

InterfaceFile::InterfaceFile(InterfaceFileList& if_file_list, const Period& period)
:m_pLog(base::Log::Instance())
,m_pIFFileList(&if_file_list)
,m_pPeriod(&period)
,m_pathSeq(0)
,m_days(0)
,m_hour(0)
,m_fileBlanksize(0)
{
}

InterfaceFile::InterfaceFile(const InterfaceFile& iff)
:m_pLog(base::Log::Instance())
,m_pIFFileList(iff.m_pIFFileList)
,m_pPeriod(iff.m_pPeriod)
,m_pathSeq(iff.m_pathSeq)
,m_channel(iff.m_channel)
,m_fileName(iff.m_fileName)
,m_days(iff.m_days)
,m_hour(iff.m_hour)
,m_fileBlanksize(iff.m_fileBlanksize)
,m_setFileNameEx(iff.m_setFileNameEx)
,m_estimatedTime(iff.m_estimatedTime)
{
}

InterfaceFile::~InterfaceFile()
{
	base::Log::Release();
}

InterfaceFile& InterfaceFile::operator = (const InterfaceFile& iff)
{
	if ( this != &iff )
	{
		this->m_pathSeq       = iff.m_pathSeq;
		this->m_channel       = iff.m_channel;
		this->m_fileName      = iff.m_fileName;
		this->m_days          = iff.m_days;
		this->m_hour          = iff.m_hour;
		this->m_fileBlanksize = iff.m_fileBlanksize;
		this->m_setFileNameEx = iff.m_setFileNameEx;
		this->m_estimatedTime = iff.m_estimatedTime;
	}

	return *this;
}

void InterfaceFile::Init(const std::string& fmt) throw(base::Exception)
{
	Explain(fmt);

	Check();

	Expand();
}

void InterfaceFile::Explain(const std::string& fmt) throw(base::Exception)
{
	m_pLog->Output("[INTERFACE_FILE] Explain: %s", fmt.c_str());

	std::vector<std::string> vec_str;
	base::PubStr::Str2StrVector(fmt, ",", vec_str);

	if ( vec_str.size() != S_INTERFACE_FILE_FMT_SIZE )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "接口文件规则格式不正确: SIZE=[%lu], less than [%u] [FILE:%s, LINE:%d]", vec_str.size(), S_INTERFACE_FILE_FMT_SIZE, __FILE__, __LINE__);
	}

	// 目录序号
	if ( !base::PubStr::Str2Int(vec_str[0], m_pathSeq) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "目录序号不是有效字符: [%s] [FILE:%s, LINE:%d]", vec_str[0].c_str(), __FILE__, __LINE__);
	}

	// 渠道
	m_channel = base::PubStr::UpperB(vec_str[1]);

	// 文件名
	m_fileName = base::PubStr::UpperB(vec_str[2]);

	// 延迟天数
	if ( !base::PubStr::Str2Int(vec_str[3], m_days) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "延迟天数不是有效字符: [%s] [FILE:%s, LINE:%d]", vec_str[3].c_str(), __FILE__, __LINE__);
	}

	// 预计到达时间点：小时
	if ( !base::PubStr::Str2Int(vec_str[4], m_hour) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "到达时间点不是有效字符: [%s] [FILE:%s, LINE:%d]", vec_str[4].c_str(), __FILE__, __LINE__);
	}

	// 文件为空的大小
	if ( !base::PubStr::Str2UInt(vec_str[5], m_fileBlanksize) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "文件为空的大小不是有效字符: [%s] [FILE:%s, LINE:%d]", vec_str[5].c_str(), __FILE__, __LINE__);
	}
}

void InterfaceFile::Check() throw(base::Exception)
{
	// 目录序号是否合法
	if ( !m_pIFFileList->IsPathSeqValid(m_pathSeq) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "目录序号超越范围: [%d] [FILE:%s, LINE:%d]", m_pathSeq, __FILE__, __LINE__);
	}

	// 延迟天数是否合法
	if ( m_days < 0 )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "延迟天数非法: [%d] [FILE:%s, LINE:%d]", m_days, __FILE__, __LINE__);
	}

	// 到达时间点是否合法
	if ( m_hour < 0 || m_hour > 23 )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "到达时间点非法: [%d] [FILE:%s, LINE:%d]", m_hour, __FILE__, __LINE__);
	}

	// 文件为空的大小是否合法
	if ( m_fileBlanksize < 0 )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "文件为空的大小非法: [%d] [FILE:%s, LINE:%d]", m_fileBlanksize, __FILE__, __LINE__);
	}
}

void InterfaceFile::Expand() throw(base::Exception)
{
	ExpandEstimatedTime();

	ExpandFileNameSet();
}

void InterfaceFile::ExpandEstimatedTime() throw(base::Exception)
{
	base::SimpleTime st_period = m_pPeriod->GetDay_ST();

	// 生成预计到达时间
	long long ll_time = 0;
	std::string day = base::PubTime::TheDatePlusDays(st_period.GetYear(), st_period.GetMon(), st_period.GetDay(), m_days);
	base::PubStr::SetFormatString(day, "%s%02d0000", day.c_str(), m_hour);

	if ( !base::PubStr::Str2LLong(day, ll_time) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "账期时间非法: [%s] [FILE:%s, LINE:%d]", day.c_str(), __FILE__, __LINE__);
	}

	if ( !m_estimatedTime.Set(ll_time, true) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "设置预计文件到达时间失败: [%lld] [FILE:%s, LINE:%d]", ll_time, __FILE__, __LINE__);
	}
}

void InterfaceFile::ExpandFileNameSet() throw(base::Exception)
{
	m_setFileNameEx.clear();
	std::string period_day = m_pPeriod->GetDay();

}

