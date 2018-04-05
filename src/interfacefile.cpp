#include "interfacefile.h"
#include "interfacefilelist.h"
#include "errorcode.h"
#include "log.h"
#include "pubtime.h"
#include "pubstr.h"

InterfaceFile::InterfaceFile(InterfaceFileList& if_file_list)
:m_pLog(base::Log::Instance())
,m_pIFFileList(&if_file_list)
,m_pathSeq(0)
,m_days(0)
,m_hour(0)
,m_fileBlanksize(0)
{
}

InterfaceFile::~InterfaceFile()
{
	base::Log::Release();
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
	if ( m_pathSeq < 1 || m_pathSeq > m_pIFFileList->GetPathSize() )
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
	std::string day_time = m_pIFFileList->GetPeriodDay();

	int year = 0;
	if ( !base::PubStr::Str2Int(day_time.substr(0, 4), year) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "账期时间的年份无效: [%s] [FILE:%s, LINE:%d]", day_time.substr(0, 4).c_str(), __FILE__, __LINE__);
	}

	int mon = 0;
	if ( !base::PubStr::Str2Int(day_time.substr(4, 2), mon) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "账期时间的月份无效: [%s] [FILE:%s, LINE:%d]", day_time.substr(4, 2).c_str(), __FILE__, __LINE__);
	}

	int day = 0;
	if ( !base::PubStr::Str2Int(day_time.substr(6, 2), day) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "账期时间的日份无效: [%s] [FILE:%s, LINE:%d]", day_time.substr(6, 2).c_str(), __FILE__, __LINE__);
	}

	// 生成预计到达时间
	long long ll_time = 0;
	day_time = base::PubTime::TheDatePlusDays(year, mon, day, m_days);
	if ( !base::PubStr::Str2LLong(day_time, ll_time) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "账期时间非法: [%s] [FILE:%s, LINE:%d]", day_time.c_str(), __FILE__, __LINE__);
	}

	ll_time <<= 6;
	ll_time += (m_hour * 10000);

	if ( !m_estimatedTime.Set(ll_time) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "设置预计文件到达时间失败: [%lld] [FILE:%s, LINE:%d]", ll_time, __FILE__, __LINE__);
	}
}

void InterfaceFile::ExpandFileNameSet() throw(base::Exception)
{
	std::string day_time = m_pIFFileList->GetPeriodDay();

	if ( 
}

