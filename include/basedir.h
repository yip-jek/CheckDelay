#pragma once

#include <string>

typedef struct __dirstream DIR;

namespace base
{

// 基础 Directory 类
class BaseDir
{
private:    // noncopyable
	BaseDir(const BaseDir& );
	const BaseDir& operator = (const BaseDir& );

public:
	explicit BaseDir(const std::string& path = std::string());
	virtual ~BaseDir();

	// 目录下的文件类型
	enum DirFileType
	{
		DFT_Unknown    = 0,			// 未知文件
		DFT_DIR        = 1,			// 目录文件
		DFT_REG        = 2,			// 常规文件
		DFT_LNK        = 3,			// 链接文件
	};

	static std::string DescribeFileType(DirFileType type);

	// 目录末尾加上斜杠
	static void DirWithSlash(std::string& dir);

	// 目录是否存在
	static bool IsDirExist(const std::string& dir_path);

	// 创建目录
	static bool MakeDir(const std::string& dir_path);

	// 创建全路径
	static bool CreateFullPath(const std::string& path);

public:
	// 设置目录
	virtual bool SetPath(const std::string& path);

	virtual std::string GetPath() const;

	// 打开目录
	// std::string* pStrError：接收错误信息
	virtual bool Open(std::string* pStrError);

	// 关闭目录
	virtual void Close();

	// 获取目录下的文件名
	virtual bool GetFileName(std::string& file_name, DirFileType& type, long long& size, time_t& time);

	// 获取目录下的文件全名（包含目录）
	virtual bool GetFullName(std::string& full_name, DirFileType& type, long long& size, time_t& time);

protected:
	// 创建目录 (递归)
	static bool MakeDirRecursive(const std::string& dir_path);

protected:
	DIR*		m_pDir;
	std::string	m_strPath;
};

}	// namespace base

