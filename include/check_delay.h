#pragma once

#include <string>
#include <vector>
#include "exception.h"
#include "config.h"
#include "basefile.h"

namespace base
{
class Log;
}

class FullDir;

class CheckDelay
{
public:
	CheckDelay();
	~CheckDelay();

	static std::string GetCfgLogPath(char* p_cfgfile) throw(base::Exception);

public:
	void LoadConfig(char* p_cfgfile) throw(base::Exception);

	void Init() throw(base::Exception);

	void Run() throw(base::Exception);

private:
	void LoadInputFilePath() throw(base::Exception);

	void LogOutputInputFilePath();

	void InitInputDir() throw(base::Exception);

	void ReleaseInputDir();

	void CheckFDir(FullDir* pFDir) throw(base::Exception);

private:
	base::Log*   m_pLog;
	base::Config m_cfg;

private:
	std::string              m_dbName;
	std::string              m_dbUsr;
	std::string              m_dbPwd;
	bool                     m_dirVisible;
	bool                     m_recurse;
	std::string              m_interfaceFileList;
	unsigned int             m_uPathSize;
	std::vector<std::string> m_vecFilePath;
	std::string              m_outputPath;
	std::string              m_outputTab;

private:
	std::vector<FullDir*> m_vecInputFDir;
	base::BaseFile        m_outputFile;
};

