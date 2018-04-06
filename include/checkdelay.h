#pragma once

#include <string>
#include <vector>
#include "config.h"
#include "period.h"

namespace base
{
class Log;
}

class FullDir;
class InterfaceFileList;

class CheckDelay
{
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

	// 初始化输入目录
	void InitInputDir() throw(base::Exception);

	// 初始化账期时间
	void InitPeriod() throw(base::Exception);

	// 初始化 InterfaceFileList
	void InitInterfaceFileList() throw(base::Exception);

	// 释放输入目录资源
	void ReleaseInputDir();

	// 释放 InterfaceFileList
	void ReleaseInterfaceFileList();

	void CheckFDir(FullDir* pFDir) throw(base::Exception);

private:
	base::Log*   m_pLog;
	base::Config m_cfg;

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
	std::vector<FullDir*>    m_vecInputFDir;			// 输入目录列表
	Period                   m_period;
	InterfaceFileList*       m_pIffList;
};

