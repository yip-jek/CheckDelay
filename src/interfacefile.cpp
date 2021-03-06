#include "interfacefile.h"
#include "interfacefilelist.h"
#include "errorcode.h"
#include "log.h"
#include "pubtime.h"
#include "pubstr.h"
#include "fulldir.h"
#include "period.h"
#include "optionset.h"
#include "iffilestate.h"
#include "outputfilestate.h"

const char* const InterfaceFile::S_CHANNEL_TAG = "CHANNEL";

InterfaceFile::InterfaceFile(InterfaceFileList& if_file_list, const Period& period)
:m_pLog(base::Log::Instance())
,m_pIFFileList(&if_file_list)
,m_pPeriod(&period)
,m_stNow(base::SimpleTime::Now())
,m_pathSeq(0)
,m_days(0)
,m_hour(0)
,m_min(0)
,m_fileBlanksize(0)
,m_isMonPeriod(false)
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
,m_min(iff.m_min)
,m_fileBlanksize(iff.m_fileBlanksize)
,m_isMonPeriod(iff.m_isMonPeriod)
,m_mapFileNameEx(iff.m_mapFileNameEx)
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
		this->m_min           = iff.m_min;
		this->m_fileBlanksize = iff.m_fileBlanksize;
		this->m_isMonPeriod   = iff.m_isMonPeriod;
		this->m_mapFileNameEx = iff.m_mapFileNameEx;
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

void InterfaceFile::UpdateFileState(const FDFileInfo& f_info)
{
	const std::string UPPER_FILE_NAME = base::PubStr::UpperB(f_info.file_name);
	const size_t      FILE_NAME_SIZE  = UPPER_FILE_NAME.size();

	std::string upper_iffile;
	for ( MAP_IFFILE_STATE::iterator m_it = m_mapFileNameEx.begin(); m_it != m_mapFileNameEx.end(); ++m_it )
	{
		upper_iffile = base::PubStr::UpperB(m_it->first);
		const size_t IFFILE_SIZE = upper_iffile.size();

		// 完全匹配，或者前部匹配
		if ( (FILE_NAME_SIZE == IFFILE_SIZE && UPPER_FILE_NAME == upper_iffile) 
			|| (FILE_NAME_SIZE > IFFILE_SIZE && UPPER_FILE_NAME.substr(0, IFFILE_SIZE) == upper_iffile) )
		{
			IFFileState& ref_iffs = m_it->second;

			// 是否内容为空？
			if ( f_info.file_size <= (long long)m_fileBlanksize )	// 为空
			{
				// 是否延迟？
				if ( f_info.chg_time > m_estimatedTime )	// 延迟
				{
					ref_iffs.SetFileState(IFFileState::IFFSTATE_DELAY_BLANK);
				}
				else	// 不延迟
				{
					ref_iffs.SetFileState(IFFileState::IFFSTATE_BLANK);
				}
			}
			else	// 非为空
			{
				// 是否延迟？
				if ( f_info.chg_time > m_estimatedTime )	// 延迟
				{
					ref_iffs.SetFileState(IFFileState::IFFSTATE_DELAY);
				}
				else	// 不延迟
				{
					ref_iffs.SetFileState(IFFileState::IFFSTATE_NORMAL);
				}
			}

			ref_iffs.m_fileSize = f_info.file_size;
			ref_iffs.m_chgtime  = f_info.chg_time.TimeStamp();
			break;
		}
	}
}

void InterfaceFile::ExportOutputFile(OutputFileState& ofs, std::vector<OutputFileState>& vec_ofs)
{
	for ( MAP_IFFILE_STATE::iterator it = m_mapFileNameEx.begin(); it != m_mapFileNameEx.end(); ++it )
	{
		IFFileState& ref_iffs = it->second;

		ofs.file_name     = it->first;
		ofs.file_size     = ref_iffs.m_fileSize;
		ofs.file_chgtime  = ref_iffs.m_chgtime;
		ofs.iff_state     = ref_iffs.GetFileState();
		ofs.state_missing = ref_iffs.GetStateMissing();
		ofs.state_blank   = ref_iffs.GetStateBlank();
		ofs.state_delay   = ref_iffs.GetStateDelay();

		vec_ofs.push_back(ofs);
	}
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
	// 保持原有大小写的格式
	//m_fileName = base::PubStr::UpperB(vec_str[2]);
	m_fileName = vec_str[2];

	// 延迟天数
	if ( !base::PubStr::Str2Int(vec_str[3], m_days) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "延迟天数不是有效字符: [%s] [FILE:%s, LINE:%d]", vec_str[3].c_str(), __FILE__, __LINE__);
	}

	// 预计到达时间点
	SplitHourMin(vec_str[4]);

	// 文件为空的大小
	if ( !base::PubStr::Str2UInt(vec_str[5], m_fileBlanksize) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "文件为空的大小不是有效字符: [%s] [FILE:%s, LINE:%d]", vec_str[5].c_str(), __FILE__, __LINE__);
	}
}

void InterfaceFile::SplitHourMin(const std::string& hm) throw(base::Exception)
{
	// 格式：HHMI 或者 HMI
	// 例如：0800 或者 800，均表示 08:00:00
	const size_t HM_SIZE = hm.size();
	if ( HM_SIZE != 3 && HM_SIZE != 4 )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "到达时间点“小时分钟”格式不正确: [%s] [FILE:%s, LINE:%d]", hm.c_str(), __FILE__, __LINE__);
	}

	std::string hhmi = hm;

	// 3位时间在前面补'0'
	if ( HM_SIZE == 3 )
	{
		hhmi.insert(0, "0");
	}

	// 小时
	if ( !base::PubStr::Str2Int(hhmi.substr(0, 2), m_hour) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "到达小时数不是有效字符: [%s] [FILE:%s, LINE:%d]", hhmi.substr(0, 2).c_str(), __FILE__, __LINE__);
	}

	// 分钟
	if ( !base::PubStr::Str2Int(hhmi.substr(2, 2), m_min) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "到达分钟数不是有效字符: [%s] [FILE:%s, LINE:%d]", hhmi.substr(2, 2).c_str(), __FILE__, __LINE__);
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

	// 到达小时是否合法
	if ( m_hour < 0 || m_hour > 23 )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "到达时间点（小时）非法: [%d] [FILE:%s, LINE:%d]", m_hour, __FILE__, __LINE__);
	}

	// 到达分钟是否合法
	if ( m_min < 0 || m_min > 59 )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "到达时间点（分钟）非法: [%d] [FILE:%s, LINE:%d]", m_min, __FILE__, __LINE__);
	}

	// 文件为空的大小是否合法
	if ( m_fileBlanksize < 0 )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "文件为空的大小非法: [%d] [FILE:%s, LINE:%d]", m_fileBlanksize, __FILE__, __LINE__);
	}

	CheckMonPeriod();
}

void InterfaceFile::CheckMonPeriod()
{
	std::string  period_fmt;
	unsigned int fmt_size = 0;

	if ( OptionSet::GetSubstitute(m_fileName, 0, '[', ']', period_fmt, fmt_size) >= 0 )
	{
		m_isMonPeriod = Period::IsMonType(period_fmt);
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
	long long        ll_time   = 0;
	std::string      str_time;

	// 生成预计到达时间
	if ( m_isMonPeriod )	// 月账期
	{
		// 月账期，账期日都是从 1 号开始算！
		// 因此，迟延天数默认减去 1 天
		int off_days = m_days - 1;
		if ( off_days >= 0 )
		{
			str_time = base::PubTime::TheDatePlusDays(st_period.GetYear(), st_period.GetMon(), 1, off_days);
		}
		else
		{
			str_time = base::PubTime::TheDateMinusDays(st_period.GetYear(), st_period.GetMon(), 1, -off_days);
		}
	}
	else	// 日账期
	{
		str_time = base::PubTime::TheDatePlusDays(st_period.GetYear(), st_period.GetMon(), st_period.GetDay(), m_days);
	}

	base::PubStr::SetFormatString(str_time, "%s%02d%02d00", str_time.c_str(), m_hour, m_min);
	if ( !base::PubStr::Str2LLong(str_time, ll_time) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "账期时间非法: [%s] [FILE:%s, LINE:%d]", str_time.c_str(), __FILE__, __LINE__);
	}

	if ( !m_estimatedTime.Set(ll_time, true) )
	{
		throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "设置预计文件到达时间失败: [%lld] [FILE:%s, LINE:%d]", ll_time, __FILE__, __LINE__);
	}

	m_pLog->Output("[INTERFACE_FILE] Estimated time: %s", m_estimatedTime.TimeStamp().c_str());
}

void InterfaceFile::ExpandFileNameSet() throw(base::Exception)
{
	m_mapFileNameEx.clear();
	if ( IsEstimatedTimeLater() )
	{
		m_pLog->Output("[INTERFACE_FILE] Not to expand file name set !");
		return;
	}

	std::string file_name;
	if ( m_isMonPeriod )	// 月账期
	{
		base::SimpleTime st_period = m_pPeriod->GetDay_ST();
		std::string str_day = base::PubTime::TheDateMinusMonths(st_period.GetYear(), st_period.GetMon(), 1) + "01";

		// 月账期，采集新账期：上月1号
		Period p;
		p.Init(str_day);

		file_name = ExpandFileNamePeriod(&p);
	}
	else
	{
		file_name = ExpandFileNamePeriod(m_pPeriod);
	}

	ExpandFileNameOptions(file_name);
}

bool InterfaceFile::IsEstimatedTimeLater()
{
	if ( m_estimatedTime > m_stNow )
	{
		m_pLog->Output("<WARNING> [INTERFACE_FILE] Estimated time is later than NOW: ESTIMATED_TIME=[%s], NOW=[%s]", m_estimatedTime.TimeStamp().c_str(), m_stNow.TimeStamp().c_str());
		return true;
	}

	return false;
}

std::string InterfaceFile::ExpandFileNamePeriod(const Period* p) throw(base::Exception)
{
	std::string  file_name = m_fileName;
	std::string  period_fmt;
	std::string  period_time;
	unsigned int fmt_size = 0;
	int          beg_pos  = 0;

	while ( (beg_pos = OptionSet::GetSubstitute(file_name, 0, '[', ']', period_fmt, fmt_size)) >= 0 )
	{
		period_time = p->GetDay_fmt(period_fmt);
		if ( period_time.empty() )
		{
			throw base::Exception(ERR_CHKDLY_IFFILE_INIT, "接口文件格式不正确，无法解释的账期: [%s] [FILE:%s, LINE:%d]", period_fmt.c_str(), __FILE__, __LINE__);
		}

		file_name.replace(beg_pos, fmt_size, period_time);
	}

	m_pLog->Output("[INTERFACE_FILE] Expand file name period: %s", file_name.c_str());
	return file_name;
}

void InterfaceFile::ExpandFileNameOptions(const std::string& file_name)
{
	std::vector<std::string> vec_filename(1, file_name);
	std::vector<std::string> vec_newfilename;
	std::vector<std::string> vec_fn;

	int count = 0;
	while ( !vec_filename.empty() )
	{
		const int VEC_SIZE = vec_filename.size();
		for ( int i = 0; i < VEC_SIZE; ++i )
		{
			std::string& ref_filename = vec_filename[i];
			if ( ExpandOneOption(ref_filename, vec_fn) )
			{
				vec_newfilename.insert(vec_newfilename.end(), vec_fn.begin(), vec_fn.end());
			}
			else
			{
				// 缺省状态：缺失
				m_mapFileNameEx[ref_filename].SetFileState(IFFileState::IFFSTATE_MISSING);
				m_pLog->Output("[INTERFACE_FILE] FILE_%d) [%s]", ++count, ref_filename.c_str());
			}
		}

		vec_newfilename.swap(vec_filename);
		std::vector<std::string>().swap(vec_newfilename);
	}
}

bool InterfaceFile::ExpandOneOption(const std::string& file_name, std::vector<std::string>& vec_fn)
{
	std::string  option;
	unsigned int size = 0;

	int beg_pos = OptionSet::GetSubstitute(file_name, 0, '{', '}', option, size);
	if ( beg_pos >= 0 )
	{
		OptionSet* pOps = m_pIFFileList->GetOptionSet(option);
		pOps->Expand(file_name, vec_fn);
		return true;
	}

	return false;
}

