#include "checkdelay.h"
#include "errorcode.h"
#include "basefile.h"
#include "fulldir.h"
#include "log.h"
#include "pubstr.h"
#include "simpletime.h"

CheckDelay::CheckDelay()
:m_pLog(base::Log::Instance())
,m_dirVisible(false)
,m_recurse(false)
,m_uOptionSize(0)
,m_uPathSize(0)
{
}

CheckDelay::~CheckDelay()
{
	ReleaseInputDir();

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
	m_period     = m_cfg.GetCfgValue("COMMON", "PERIOD");

	m_stateNormal     = m_cfg.GetCfgValue("STATE", "STATE_NORMAL");
	m_stateMissing    = m_cfg.GetCfgValue("STATE", "STATE_MISSING");
	m_stateBlank      = m_cfg.GetCfgValue("STATE", "STATE_BLANK");
	m_stateDelay      = m_cfg.GetCfgValue("STATE", "STATE_DELAY");
	m_stateDelayBlank = m_cfg.GetCfgValue("STATE", "STATE_DELAY_BLANK");

	m_interFileList = m_cfg.GetCfgValue("INTERFACE", "INTERFACE_FILE_LIST");
	m_uOptionSize   = m_cfg.GetCfgUIntVal("INTERFACE", "OPTION_SIZE");
	LoadDynamicCfg("INTERFACE", "OPTION_SET", m_uOptionSize, m_vecOptions);

	m_uPathSize  = m_cfg.GetCfgUIntVal("INPUT", "PATH_SIZE");
	LoadDynamicCfg("INPUT", "FILE_PATH", m_uPathSize, m_vecFilePath);

	m_outputPath        = m_cfg.GetCfgValue("OUTPUT", "PATH");
	m_outputMissingFile = m_cfg.GetCfgValue("OUTPUT", "MISSING_FILE");
	m_outputBlankFile   = m_cfg.GetCfgValue("OUTPUT", "BLANK_FILE");
	m_outputTab         = m_cfg.GetCfgValue("OUTPUT", "TABLE_NAME");

	m_pLog->Output("[CHECK_DELAY] Load config OK.");
	LogOutConfig();
}

void CheckDelay::Init() throw(base::Exception)
{
	InitInputDir();

	m_pLog->Output("[CHECK_DELAY] Init OK.");
}

void CheckDelay::Run() throw(base::Exception)
{
	const int VEC_SIZE = m_vecInputFDir.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		CheckFDir(m_vecInputFDir[i]);
	}
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
	m_pLog->Output("[COMMON] PERIOD     : [%s]", m_period.c_str());
	m_pLog->Output("");
	m_pLog->Output("[STATE] STATE_NORMAL     : [%s]", m_stateNormal.c_str());
	m_pLog->Output("[STATE] STATE_MISSING    : [%s]", m_stateMissing.c_str());
	m_pLog->Output("[STATE] STATE_BLANK      : [%s]", m_stateBlank.c_str());
	m_pLog->Output("[STATE] STATE_DELAY      : [%s]", m_stateDelay.c_str());
	m_pLog->Output("[STATE] STATE_DELAY_BLANK: [%s]", m_stateDelayBlank.c_str());
	m_pLog->Output("");
	m_pLog->Output("[INTERFACE] INTERFACE_FILE_LIST: [%s]", m_interFileList.c_str());
	m_pLog->Output("[INTERFACE] OPTION_SIZE        : [%u]", m_uOptionSize);
	LogOutDynamicCfg("INTERFACE", "OPTION_SET", m_vecOptions);
	m_pLog->Output("");
	m_pLog->Output("[INPUT] PATH_SIZE: [%u]", m_uPathSize);
	LogOutDynamicCfg("INPUT", "FILE_PATH", m_vecFilePath);
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

void CheckDelay::CheckFDir(FullDir* pFDir) throw(base::Exception)
{
	if ( NULL == pFDir )
	{
		throw base::Exception(ERR_CHKDLY_CHECK_FAIL, "The FullDir pointer is invalid: [NULL] [FILE:%s, LINE:%d]", __FILE__, __LINE__);
	}

	std::vector<FDFileInfo> vec_fileinfo;
	pFDir->GetFileList(vec_fileinfo);

	const int VEC_SIZE = vec_fileinfo.size();
	m_pLog->Output("[CHECK_DELAY] Num of file(s): %d", VEC_SIZE);

	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		FDFileInfo& ref_fi = vec_fileinfo[i];

		m_pLog->Output("[CHECK_DELAY] %s: [%s], %s, %lld, %s", ref_fi.parent_path.c_str(), base::BaseDir::DescribeFileType(ref_fi.file_type).c_str(), ref_fi.file_name.c_str(), ref_fi.file_size, ref_fi.chg_time.TimeStamp().c_str());
	}
}

