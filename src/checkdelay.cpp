#include "checkdelay.h"
#include "errorcode.h"
#include "basefile.h"
#include "fulldir.h"
#include "log.h"
#include "pubstr.h"
#include "simpletime.h"
#include "checkdb2.h"
#include "interfacefile.h"
#include "interfacefilelist.h"
#include "optionset.h"
#include "outputfilestate.h"

const char* const CheckDelay::S_OPTION_SET = "OPTION_SET";
const char* const CheckDelay::S_FILE_PATH  = "FILE_PATH";

CheckDelay::CheckDelay()
:m_pLog(base::Log::Instance())
,m_dirVisible(false)
,m_recurse(false)
,m_uOptionSize(0)
,m_uPathSize(0)
,m_pDB2(NULL)
,m_pIffList(NULL)
{
}

CheckDelay::~CheckDelay()
{
	ReleaseDB2();
	ReleaseInputDir();
	ReleaseInterfaceFileList();

	base::Log::Release();
}

std::string CheckDelay::GetCfgLogPath(char* p_cfgfile) throw(base::Exception)
{
	base::Config t_cfg;
	t_cfg.SetCfgFile(p_cfgfile);
	t_cfg.RegisterItem("SYS", "LOG_PATH");
	t_cfg.ReadConfig();

	return t_cfg.GetCfgValue("SYS", "LOG_PATH");
}

void CheckDelay::LoadConfig(char* p_cfgfile) throw(base::Exception)
{
	m_cfg.SetCfgFile(p_cfgfile);

	m_cfg.RegisterItem("DATABASE", "DB_NAME");
	m_cfg.RegisterItem("DATABASE", "USER_NAME");
	m_cfg.RegisterItem("DATABASE", "PASSWORD");

	m_cfg.RegisterItem("COMMON", "DIR_VISIBLE");
	m_cfg.RegisterItem("COMMON", "RECURSE");
	m_cfg.RegisterItem("COMMON", "PERIOD");

	m_cfg.RegisterItem("STATE", "STATE_NORMAL");
	m_cfg.RegisterItem("STATE", "STATE_MISSING");
	m_cfg.RegisterItem("STATE", "STATE_BLANK");
	m_cfg.RegisterItem("STATE", "STATE_DELAY");
	m_cfg.RegisterItem("STATE", "STATE_DELAY_BLANK");

	m_cfg.RegisterItem("INTERFACE", "INTERFACE_FILE_LIST");
	m_cfg.RegisterItem("INTERFACE", "OPTION_SIZE");

	m_cfg.RegisterItem("INPUT", "PATH_SIZE");

	m_cfg.RegisterItem("OUTPUT", "PATH");
	m_cfg.RegisterItem("OUTPUT", "MISSING_FILE");
	m_cfg.RegisterItem("OUTPUT", "BLANK_FILE");
	m_cfg.RegisterItem("OUTPUT", "TABLE_NAME");

	m_cfg.ReadConfig();

	m_dbName = m_cfg.GetCfgValue("DATABASE", "DB_NAME");
	m_dbUsr  = m_cfg.GetCfgValue("DATABASE", "USER_NAME");
	m_dbPwd  = m_cfg.GetCfgValue("DATABASE", "PASSWORD");

	m_dirVisible = m_cfg.GetCfgBoolVal("COMMON", "DIR_VISIBLE");
	m_recurse    = m_cfg.GetCfgBoolVal("COMMON", "RECURSE");
	m_periodDay  = m_cfg.GetCfgValue("COMMON", "PERIOD");

	m_stateNormal     = m_cfg.GetCfgValue("STATE", "STATE_NORMAL");
	m_stateMissing    = m_cfg.GetCfgValue("STATE", "STATE_MISSING");
	m_stateBlank      = m_cfg.GetCfgValue("STATE", "STATE_BLANK");
	m_stateDelay      = m_cfg.GetCfgValue("STATE", "STATE_DELAY");
	m_stateDelayBlank = m_cfg.GetCfgValue("STATE", "STATE_DELAY_BLANK");

	m_interFileList = m_cfg.GetCfgValue("INTERFACE", "INTERFACE_FILE_LIST");
	m_uOptionSize   = m_cfg.GetCfgUIntVal("INTERFACE", "OPTION_SIZE");
	LoadDynamicCfg("INTERFACE", S_OPTION_SET, m_uOptionSize, m_vecOptions);

	m_uPathSize  = m_cfg.GetCfgUIntVal("INPUT", "PATH_SIZE");
	LoadDynamicCfg("INPUT", S_FILE_PATH, m_uPathSize, m_vecFilePath);

	m_outputPath        = m_cfg.GetCfgValue("OUTPUT", "PATH");
	m_outputMissingFile = m_cfg.GetCfgValue("OUTPUT", "MISSING_FILE");
	m_outputBlankFile   = m_cfg.GetCfgValue("OUTPUT", "BLANK_FILE");
	m_outputTab         = m_cfg.GetCfgValue("OUTPUT", "TABLE_NAME");

	m_pLog->Output("[CHECK_DELAY] Load config OK.");
	LogOutConfig();
}

void CheckDelay::Init() throw(base::Exception)
{
	InitDB2();
	InitInputDir();
	InitPeriod();
	InitInterfaceFileList();
	InitOutput();

	m_pLog->Output("[CHECK_DELAY] Init OK.");
}

void CheckDelay::Run() throw(base::Exception)
{
	TraverseInputDir();
	OutputResult();
}

void CheckDelay::LogOutConfig()
{
	m_pLog->Output(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> [CONFIG] >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
	m_pLog->Output("[DATABASE] DB_NAME: [%s]", m_dbName.c_str());
	m_pLog->Output("[DATABASE] DB_USER: [%s]", m_dbUsr.c_str());
	m_pLog->Output("[DATABASE] DB_PWD : [%s]", m_dbPwd.c_str());
	m_pLog->Output("");
	m_pLog->Output("[COMMON] DIR_VISIBLE: [%s]", (m_dirVisible ? "YES" : "NO"));
	m_pLog->Output("[COMMON] RECURSE    : [%s]", (m_recurse ? "YES" : "NO"));
	m_pLog->Output("[COMMON] PERIOD     : [%s]", m_periodDay.c_str());
	m_pLog->Output("");
	m_pLog->Output("[STATE] STATE_NORMAL     : [%s]", m_stateNormal.c_str());
	m_pLog->Output("[STATE] STATE_MISSING    : [%s]", m_stateMissing.c_str());
	m_pLog->Output("[STATE] STATE_BLANK      : [%s]", m_stateBlank.c_str());
	m_pLog->Output("[STATE] STATE_DELAY      : [%s]", m_stateDelay.c_str());
	m_pLog->Output("[STATE] STATE_DELAY_BLANK: [%s]", m_stateDelayBlank.c_str());
	m_pLog->Output("");
	m_pLog->Output("[INTERFACE] INTERFACE_FILE_LIST: [%s]", m_interFileList.c_str());
	m_pLog->Output("[INTERFACE] OPTION_SIZE        : [%u]", m_uOptionSize);
	LogOutDynamicCfg("INTERFACE", S_OPTION_SET, m_vecOptions);
	m_pLog->Output("");
	m_pLog->Output("[INPUT] PATH_SIZE: [%u]", m_uPathSize);
	LogOutDynamicCfg("INPUT", S_FILE_PATH, m_vecFilePath);
	m_pLog->Output("");
	m_pLog->Output("[OUTPUT] PATH        : [%s]", m_outputPath.c_str());
	m_pLog->Output("[OUTPUT] MISSING_FILE: [%s]", m_outputMissingFile.c_str());
	m_pLog->Output("[OUTPUT] BLANK_FILE  : [%s]", m_outputBlankFile.c_str());
	m_pLog->Output("[OUTPUT] TABLE_NAME  : [%s]", m_outputTab.c_str());
	m_pLog->Output("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}

void CheckDelay::LoadDynamicCfg(const std::string& seg, const std::string& item_prefix, unsigned int size, std::vector<std::string>& v_items) throw(base::Exception)
{
	if ( size <= 0 )
	{
		throw base::Exception(ERR_CHKDLY_LOAD_FAIL, "The config item size is invalid: [%u] [FILE:%s, LINE:%d]", size, __FILE__, __LINE__);
	}

	std::string              item_name;
	std::vector<std::string> vec_items;

	for ( unsigned int u = 0; u < size; ++u )
	{
		base::PubStr::SetFormatString(item_name, "%s_%u", item_prefix.c_str(), u+1);
		m_cfg.RegisterItem(seg, item_name);
	}

	m_cfg.ReadConfig();
	for ( unsigned int u = 0; u < size; ++u )
	{
		base::PubStr::SetFormatString(item_name, "%s_%u", item_prefix.c_str(), u+1);
		vec_items.push_back(m_cfg.GetCfgValue(seg, item_name));
	}

	vec_items.swap(v_items);
}

void CheckDelay::LogOutDynamicCfg(const std::string& seg, const std::string& item_prefix, const std::vector<std::string>& v_items)
{
	std::string item_name;

	const int VEC_SIZE = v_items.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		base::PubStr::SetFormatString(item_name, "%s_%d", item_prefix.c_str(), i+1);
		m_pLog->Output("[%s] %s: [%s]", seg.c_str(), item_name.c_str(), v_items[i].c_str());
	}
}

void CheckDelay::InitDB2() throw(base::Exception)
{
	ReleaseDB2();

	m_pDB2 = new CheckDB2(m_dbName, m_dbUsr, m_dbPwd);
	if ( NULL == m_pDB2 )
	{
		throw base::Exception(ERR_CHKDLY_INIT_FAIL, "Operate new CheckDB2() failed: 无法申请到内存空间! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
	}

	m_pDB2->Connect();
	m_pDB2->SetOutputTab(m_outputTab);
}

void CheckDelay::InitInputDir() throw(base::Exception)
{
	ReleaseInputDir();

	const int VEC_SIZE = m_vecFilePath.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		FullDir* pFDir = new FullDir();
		if ( NULL == pFDir )
		{
			throw base::Exception(ERR_CHKDLY_INIT_FAIL, "Operate new FullDir() failed: 无法申请到内存空间! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
		}
		m_vecInputFDir.push_back(pFDir);

		pFDir->SetDirVisible(m_dirVisible);
		pFDir->SetRecurse(m_recurse);

		if ( !pFDir->SetPath(m_vecFilePath[i]) )
		{
			throw base::Exception(ERR_CHKDLY_INIT_FAIL, "The input file path \"%s\" is not ready! [FILE:%s, LINE:%d]", m_vecFilePath[i].c_str(), __FILE__, __LINE__);
		}
	}
}

void CheckDelay::InitPeriod() throw(base::Exception)
{
	m_period.Init(m_periodDay);
	m_pLog->Output("[CHECK_DELAY] Period: [%s] -> [%s]", m_periodDay.c_str(), m_period.GetDay().c_str());
}

void CheckDelay::InitInterfaceFileList() throw(base::Exception)
{
	ReleaseInterfaceFileList();

	m_pIffList = new InterfaceFileList(m_uPathSize, m_period);
	m_pIffList->ImportOptions(m_vecOptions);
	m_pIffList->ImportFile(m_interFileList);
}

void CheckDelay::ReleaseDB2()
{
	if ( m_pDB2 != NULL )
	{
		m_pDB2->Disconnect();

		delete m_pDB2;
		m_pDB2 = NULL;
	}
}

void CheckDelay::ReleaseInputDir()
{
	if ( m_vecInputFDir.empty() )
	{
		return;
	}

	const int VEC_SIZE = m_vecInputFDir.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		delete m_vecInputFDir[i];
	}

	std::vector<FullDir*>().swap(m_vecInputFDir);
}

void CheckDelay::ReleaseInterfaceFileList()
{
	if ( m_pIffList != NULL )
	{
		delete m_pIffList;
		m_pIffList = NULL;
	}
}

void CheckDelay::InitOutput() throw(base::Exception)
{
	CloseOutputFile(m_mapOutputMissing);
	CloseOutputFile(m_mapOutputBlank);

	InitOutputPath();

	TransOutputPeriod(m_outputMissingFile);
	TransOutputPeriod(m_outputBlankFile);

	std::set<std::string> s_chann;
	m_pIffList->GetChannels(s_chann);

	InitOutputChannel(m_outputMissingFile, s_chann, m_mapOutputMissing);
	InitOutputChannel(m_outputBlankFile, s_chann, m_mapOutputBlank);
}

void CheckDelay::InitOutputPath() throw(base::Exception)
{
	if ( !base::BaseDir::CreateFullPath(m_outputPath) )
	{
		throw base::Exception(ERR_CHKDLY_INIT_FAIL, "The output path is not ready: [%s] [FILE:%s, LINE:%d]", m_outputPath.c_str(), __FILE__, __LINE__);
	}

	base::BaseDir::DirWithSlash(m_outputPath);
}

void CheckDelay::TransOutputPeriod(std::string& file_name)
{
	std::string  period;
	unsigned int fmt_size = 0;
	int          beg_pos  = 0;

	while ( (beg_pos = OptionSet::GetSubstitute(file_name, beg_pos, '[', ']', period, fmt_size)) >= 0 )
	{
		period = m_period.GetDay_fmt(period);
		if ( period.empty() )
		{
			beg_pos += fmt_size;
		}
		else
		{
			file_name.replace(beg_pos, fmt_size, period);
			beg_pos += period.size();
		}
	}

	m_pLog->Output("[CHECK_DELAY] Transfer output file period: %s", file_name.c_str());
}

void CheckDelay::InitOutputChannel(const std::string& file_name, std::set<std::string> set_chann, MAP_CHANN_OUTPUT& map_bf) throw(base::Exception)
{
	std::string  str;
	unsigned int size    = 0;
	int          beg_pos = 0;

	while ( (beg_pos = OptionSet::GetSubstitute(file_name, beg_pos, '[', ']', str, size)) >= 0 )
	{
		if ( base::PubStr::UpperB(str) == InterfaceFile::S_CHANNEL_TAG )
		{
			break;
		}
	}

	if ( beg_pos < 0 )
	{
		throw base::Exception(ERR_CHKDLY_INIT_FAIL, "找不到输出文件名的渠道标识: [%s] [FILE:%s, LINE:%d]", InterfaceFile::S_CHANNEL_TAG, __FILE__, __LINE__);
	}

	for ( std::set<std::string>::iterator s_it = set_chann.begin(); s_it != set_chann.end(); ++s_it )
	{
		str = file_name;
		str.replace(beg_pos, size, *s_it);
		str = m_outputPath + str;

		base::BaseFile& ref_bf = map_bf[*s_it];
		if ( !ref_bf.Open(str, true) )
		{
			throw base::Exception(ERR_CHKDLY_INIT_FAIL, "Open output file failed: [%s] [FILE:%s, LINE:%d]", str.c_str(), __FILE__, __LINE__);
		}

		if ( !ref_bf.ReadyToWrite() )
		{
			throw base::Exception(ERR_CHKDLY_INIT_FAIL, "The output file is not ready to write: [%s] [FILE:%s, LINE:%d]", str.c_str(), __FILE__, __LINE__);
		}

		m_pLog->Output("[CHECK_DELAY] Output file: %s, [CHANNEL=%s]", str.c_str(), s_it->c_str());
	}
}

void CheckDelay::TraverseInputDir() throw(base::Exception)
{
	const int VEC_SIZE = m_vecInputFDir.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		FullDir* p_fdir = m_vecInputFDir[i];
		m_pLog->Output("[CHECK_DELAY] Traverse input path [%d]: %s", i+1, p_fdir->GetPath().c_str());

		std::vector<FDFileInfo> vec_fileinfo;
		p_fdir->GetFileList(vec_fileinfo);
		m_pLog->Output("[CHECK_DELAY] Num of file(s): %lu", vec_fileinfo.size());

		m_pIffList->CompareFile(i+1, vec_fileinfo);
	}
}

void CheckDelay::OutputResult()
{
	m_pIffList->ExportFileState(m_vecOutputFState);

	MakeStateDesc();
	OutputToDB2();
	OutputToFile();
}

void CheckDelay::MakeStateDesc()
{
	// 生成状态描述
	const int VEC_SIZE = m_vecOutputFState.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		OutputFileState& ref_ofs = m_vecOutputFState[i];

		switch ( ref_ofs.iff_state )
		{
		case IFFileState::IFFSTATE_NORMAL:			// 正常
			ref_ofs.state_desc = m_stateNormal;
			break;
		case IFFileState::IFFSTATE_MISSING:			// 缺失
			ref_ofs.state_desc = m_stateMissing;
			break;
		case IFFileState::IFFSTATE_BLANK:			// 内容为空
			ref_ofs.state_desc = m_stateBlank;
			break;
		case IFFileState::IFFSTATE_DELAY:			// 延迟
			ref_ofs.state_desc = m_stateDelay;
			break;
		case IFFileState::IFFSTATE_DELAY_BLANK:		// 延迟，内容为空
			ref_ofs.state_desc = m_stateDelayBlank;
			break;
		}
	}
}

void CheckDelay::OutputToDB2()
{
	m_pDB2->MergeResultData(m_vecOutputFState);
	m_pLog->Output("[CHECK_DELAY] Output result data to DB2: [%lu]", m_vecOutputFState.size());
}

void CheckDelay::OutputToFile()
{
	std::map<std::string, unsigned int> map_stat_mis;
	std::map<std::string, unsigned int> map_stat_blk;

	const std::string period_day = m_period.GetDay();
	const int         VEC_SIZE   = m_vecOutputFState.size();

	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		OutputFileState& ref_ofs = m_vecOutputFState[i];

		if ( ref_ofs.state_missing )
		{
			++map_stat_mis[ref_ofs.channel];
			m_mapOutputMissing[ref_ofs.channel].Write(period_day+"|"+ref_ofs.file_name);
		}

		if ( ref_ofs.state_blank )
		{
			++map_stat_blk[ref_ofs.channel];
			m_mapOutputBlank[ref_ofs.channel].Write(period_day+"|"+ref_ofs.file_name);
		}
	}

	for ( std::map<std::string, unsigned int>::iterator it1 = map_stat_mis.begin(); it1 != map_stat_mis.end(); ++it1 )
	{
		m_pLog->Output("[CHECK_DELAY] Output to file: CHANNEL=[%s], MISSING=[%u]", it1->first.c_str(), it1->second);
	}

	for ( std::map<std::string, unsigned int>::iterator it2 = map_stat_blk.begin(); it2 != map_stat_blk.end(); ++it2 )
	{
		m_pLog->Output("[CHECK_DELAY] Output to file: CHANNEL=[%s], BLANK=[%u]", it2->first.c_str(), it2->second);
	}

	CloseOutputFile(m_mapOutputMissing);
	CloseOutputFile(m_mapOutputBlank);
}

void CheckDelay::CloseOutputFile(MAP_CHANN_OUTPUT& map_bf)
{
	if ( map_bf.empty() )
	{
		return;
	}

	for ( MAP_CHANN_OUTPUT::iterator it = map_bf.begin(); it != map_bf.end(); ++it )
	{
		m_pLog->Output("[CHECK_DELAY] Close output file: %s", it->second.GetFilePath().c_str());

		it->second.Close();
	}

	map_bf.clear();
}

