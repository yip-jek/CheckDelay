#include "check_delay.h"
#include "errorcode.h"
#include "log.h"
#include "simpletime.h"

CheckDelay::CheckDelay()
:m_pLog(base::Log::Instance())
,m_dirVisible(false)
,m_recurse(false)
{
}

CheckDelay::~CheckDelay()
{
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

	m_cfg.RegisterItem("COMMON", "DIR_VISIBLE");
	m_cfg.RegisterItem("COMMON", "RECURSE");
	m_cfg.RegisterItem("COMMON", "FILE_PATH");
	m_cfg.RegisterItem("COMMON", "OUTPUT_PATH");

	m_cfg.ReadConfig();

	m_dirVisible = m_cfg.GetCfgBoolVal("COMMON", "DIR_VISIBLE");
	m_recurse    = m_cfg.GetCfgBoolVal("COMMON", "RECURSE");
	m_filePath   = m_cfg.GetCfgValue("COMMON", "FILE_PATH");
	m_outputPath = m_cfg.GetCfgValue("COMMON", "OUTPUT_PATH");

	m_pLog->Output("[CHECK_DELAY] Load config OK.");
}

void CheckDelay::Init() throw(base::Exception)
{
	m_inputFDir.SetDirVisible(m_dirVisible);
	m_inputFDir.SetRecurse(m_recurse);

	if ( !m_inputFDir.SetPath(m_filePath) )
	{
		throw base::Exception(ERR_CHKDLY_INIT_FAIL, "The input file path \"%s\" is not ready! [FILE:%s, LINE:%d]", m_filePath.c_str(), __FILE__, __LINE__);
	}

	m_pLog->Output("[CHECK_DELAY] Init OK.");
}

void CheckDelay::Run()
{
	std::vector<FDFileInfo> vec_fileinfo;
	m_inputFDir.GetFileList(vec_fileinfo);

	const int VEC_SIZE = vec_fileinfo.size();
	m_pLog->Output("[CHECK_DELAY] Num of file(s): %d", VEC_SIZE);

	base::SimpleTime st;
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		FDFileInfo& ref_fi = vec_fileinfo[i];
		st.Set(ref_fi.chg_time);

		m_pLog->Output("[CHECK_DELAY] %s: %s, [%s], %lld, %s", ref_fi.parent_path.c_str(), ref_fi.file_name.c_str(), base::BaseDir::DescribeFileType(ref_fi.file_type).c_str(), ref_fi.file_size, st.TimeStamp().c_str());
	}
}

