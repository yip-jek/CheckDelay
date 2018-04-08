#pragma once

#include <vector>
#include "basedir.h"
#include "exception.h"
#include "simpletime.h"

struct FDFileInfo
{
public:
	FDFileInfo(): file_type(base::BaseDir::DFT_Unknown), file_size(0)
	{}

public:
	std::string                parent_path;
	std::string                file_name;
	base::BaseDir::DirFileType file_type;
	long long                  file_size;
	base::SimpleTime           chg_time;
};

class FullDir
{
public:
	FullDir();
	~FullDir();

public:
	void SetDirVisible(bool visible);

	void SetRecurse(bool recur);

	bool SetPath(const std::string& path);

	std::string GetPath() const { return m_pathDir.GetPath(); }

	void GetFileList(std::vector<FDFileInfo>& vec_fileinfo) throw(base::Exception);

private:
	void RecurseGetFileList(std::vector<std::string>& vec_dir, std::vector<FDFileInfo>& vec_file) throw(base::Exception);

private:
	bool          m_dirVisible;
	bool          m_recurse;			// 是否递归
	base::BaseDir m_pathDir;
};

