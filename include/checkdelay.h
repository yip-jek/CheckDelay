#pragma once

#include <string>
#include <set>
#include <map>
#include <vector>
#include "config.h"
#include "period.h"

namespace base
{
class Log;
class BaseFile;
}

class CheckDB2;
class FullDir;
class InterfaceFileList;
struct OutputFileState;

class CheckDelay
{
public:
	typedef std::map<std::string, base::BaseFile>			MAP_CHANN_OUTPUT;
	typedef std::vector<OutputFileState>					VEC_OUT_FSTATE;

public:
	CheckDelay();
	~CheckDelay();

	static const char* const S_OPTION_SET;
	static const char* const S_FILE_PATH;

	static std::string GetCfgLogPath(char* p_cfgfile) throw(base::Exception);

public:
	// 载入配置
	void LoadConfig(char* p_cfgfile) throw(base::Exception);

	// 初始化
	void Init() throw(base::Exception);

	// 运行
	void Run() throw(base::Exception);

private:
	// 日志输出配置项
	void LogOutConfig();

	// 载入动态配置项
	void LoadDynamicCfg(const std::string& seg, const std::string& item_prefix, unsigned int size, std::vector<std::string>& v_items) throw(base::Exception);

	// 日志输出动态配置项
	void LogOutDynamicCfg(const std::string& seg, const std::string& item_prefix, const std::vector<std::string>& v_items);

	// 初始化数据库连接
	void InitDB2() throw(base::Exception);

	// 初始化输入目录
	void InitInputDir() throw(base::Exception);

	// 初始化账期时间
	void InitPeriod() throw(base::Exception);

	// 初始化 InterfaceFileList
	void InitInterfaceFileList() throw(base::Exception);

	// 释放数据库连接
	void ReleaseDB2();

	// 释放输入目录资源
	void ReleaseInputDir();

	// 释放 InterfaceFileList
	void ReleaseInterfaceFileList();

	// 初始化输出
	void InitOutput() throw(base::Exception);

	void InitOutputPath() throw(base::Exception);

	void TransOutputPeriod(std::string& file_name);

	void InitOutputChannel(const std::string& file_name, std::set<std::string> set_chann, MAP_CHANN_OUTPUT& map_bf) throw(base::Exception);

	void TraverseInputDir() throw(base::Exception);

	void OutputResult();

	void MakeStateDesc();

	void OutputToDB2();

	void OutputToFile();

	void CloseOutputFile(MAP_CHANN_OUTPUT& map_bf);

private:
	base::Log*               m_pLog;
	base::Config             m_cfg;

private:
	std::string              m_dbName;					// 数据库名
	std::string              m_dbUsr;					// 用户名
	std::string              m_dbPwd;					// 密码
	bool                     m_dirVisible;				// 目录是否可见
	bool                     m_recurse;					// 是否递归
	std::string              m_periodDay;				// 账期

private:
	std::string              m_stateNormal;				// 状态：正常
	std::string              m_stateMissing;			// 状态：缺失
	std::string              m_stateBlank;				// 状态：无内容
	std::string              m_stateDelay;				// 状态：延迟
	std::string              m_stateDelayBlank;			// 状态：延迟，无内容

private:
	std::string              m_interFileList;			// 接口文件清单
	unsigned int             m_uOptionSize;				// 选项个数
	std::vector<std::string> m_vecOptions;				// 选项列表
	unsigned int             m_uPathSize;				// 路径个数
	std::vector<std::string> m_vecFilePath;				// 路径列表
	std::string              m_outputPath;				// 输出路径
	std::string              m_outputMissingFile;		// 输出缺失文件
	std::string              m_outputBlankFile;			// 输出无内容文件
	std::string              m_outputTab;				// 输出库表名

private:
	CheckDB2*                m_pDB2;
	std::vector<FullDir*>    m_vecInputFDir;			// 输入目录列表
	Period                   m_period;
	InterfaceFileList*       m_pIffList;

private:
	MAP_CHANN_OUTPUT         m_mapOutputMissing;
	MAP_CHANN_OUTPUT         m_mapOutputBlank;
	VEC_OUT_FSTATE           m_vecOutputFState;
};

