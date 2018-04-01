#pragma once

#include <string>
#include "exception.h"
#include "config.h"
#include "basefile.h"
#include "fulldir.h"

namespace base
{
class Log;
}

class CheckDelay
{
public:
	CheckDelay();
	~CheckDelay();

	static std::string GetCfgLogPath(char* p_cfgfile) throw(base::Exception);

public:
	void LoadConfig(char* p_cfgfile) throw(base::Exception);

	void Init() throw(base::Exception);

	void Run();

private:

private:
	base::Log*   m_pLog;
	base::Config m_cfg;
	bool         m_dirVisible;
	bool         m_recurse;
	std::string  m_filePath;
	std::string  m_outputPath;

private:
	FullDir        m_inputFDir;
	base::BaseFile m_outputFile;
};

