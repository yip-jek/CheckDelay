#include "log.h"
#include <iostream>
#include <stdio.h>
#include "errorcode.h"
#include "simpletime.h"
#include "pubstr.h"
#include "basedir.h"

namespace base
{

int Log::_sInstances = 0;
Log* Log::_spLogger = NULL;		// single log pointer

long long Log::_sLogID = 0;
unsigned long long Log::_sMaxLogFileSize = 10*1024*1024;	// default log file size 10M

std::string Log::_sLogFilePrefix = "LOG";			// default log file prefix

Log::Log()
{
}

Log::~Log()
{
}

Log* Log::Instance()
{
	if ( NULL == _spLogger )
	{
		std::cout << "[LOG] Creating logger instance ..." << std::endl;
		_spLogger = new Log();
	}

	// 计数加 1
	++_sInstances;
	return _spLogger;
}

void Log::Release()
{
	if ( _spLogger != NULL )
	{
		// 计数减 1
		--_sInstances;

		// 计数器归 0，则释放资源
		if ( _sInstances <= 0 )
		{
			std::cout << "[LOG] Releasing logger instance ..." << std::endl;

			delete _spLogger;
			_spLogger = NULL;
		}
	}
}

bool Log::SetLogID(long long log_id)
{
	if ( log_id > 0 )
	{
		_sLogID = log_id;
		return true;
	}

	return false;
}

bool Log::ResetFileSize(unsigned long long fsize)
{
	if ( fsize > 0 )
	{
		_sMaxLogFileSize = fsize;
		return true;
	}

	return false;
}

void Log::SetLogFilePrefix(const std::string& log_prefix)
{
	if ( !log_prefix.empty() )
	{
		_sLogFilePrefix = log_prefix;
	}
}

void Log::Init() throw(Exception)
{
	if ( m_sLogPath.empty() )
	{
		throw Exception(ERR_LOG_INIT_FAIL, "[LOG] The log path is not configured! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
	}

	OpenNewLogger();
}

void Log::SetPath(const std::string& path) throw(Exception)
{
	if ( path.empty() )
	{
		throw Exception(ERR_LOG_INVALID_PATH, "[LOG] The log path is empty! [FILE:%s, LINE:%d]", __FILE__, __LINE__);
	}

	// 自动建日志路径
	if ( !BaseDir::CreateFullPath(path) )
	{
		throw Exception(ERR_LOG_CREATE_PATH_FAIL, "[LOG] The log path is invalid: %s [FILE:%s, LINE:%d]", path.c_str(), __FILE__, __LINE__);
	}

	m_sLogPath = path;
	BaseDir::DirWithSlash(m_sLogPath);
}

bool Log::Output(const char* format, ...)
{
	if ( !m_bfLogger.IsOpen() )
	{
		return false;
	}

	char buf[MAX_BUFFER_SIZE] = "";
	va_list arg_ptr;
	va_start(arg_ptr, format);
	vsnprintf(buf, sizeof(buf), format, arg_ptr);
	va_end(arg_ptr);

	std::string str_out = SimpleTime::Now().LLTimeStamp() + std::string("\x20\x20") + buf;
	m_bfLogger.Write(str_out);

	// Maximum file size
	if ( m_bfLogger.FileSize() >= _sMaxLogFileSize )
	{
		str_out = "\n\n\x20\x20\x20\x20( MAXIMUM FILE SIZE )";
		m_bfLogger.Write(str_out);
		m_bfLogger.Close();

		OpenNewLogger();
	}
	return true;
}

void Log::OpenNewLogger() throw(Exception)
{
	int log_id = 0;
	std::string fullLogPath;

	const std::string DAY_TIME = SimpleTime::Now().DayTime8();

	do
	{
		PubStr::SetFormatString(fullLogPath, "%s%s_%lld_%s_%04d.log", m_sLogPath.c_str(), _sLogFilePrefix.c_str(), _sLogID, DAY_TIME.c_str(), log_id++);

	} while ( BaseFile::IsFileExist(fullLogPath) );

	if ( !m_bfLogger.Open(fullLogPath) )
	{
		throw Exception(ERR_LOG_OPEN_LOGFILE_FAIL, "[LOG] Can not open log file: %s [FILE:%s, LINE:%d]", fullLogPath.c_str(), __FILE__, __LINE__);
	}
}

}	// namespace base

