#include "fulldir.h"
#include "errorcode.h"

FullDir::FullDir()
:m_dirVisible(false)
,m_recurse(false)
{
}

FullDir::~FullDir()
{
}

void FullDir::SetDirVisible(bool visible)
{
	m_dirVisible = visible;
}

void FullDir::SetRecurse(bool recur)
{
	m_recurse = recur;
}

bool FullDir::SetPath(const std::string& path)
{
	if ( !base::BaseDir::IsDirExist(path) )
	{
		return false;
	}

	return m_pathDir.SetPath(path);
}

void FullDir::GetFileList(std::vector<FDFileInfo>& vec_fileinfo) throw(base::Exception)
{
	const std::string DIR_PATH = m_pathDir.GetPath();

	std::string str_error;
	if ( !m_pathDir.Open(&str_error) )
	{
		throw base::Exception(ERR_CHKDLY_OPENDIR_FAIL, "Can not open the file path \"%s\": %s [FILE:%s, LINE:%d]", DIR_PATH.c_str(), str_error.c_str(), __FILE__, __LINE__);
	}

	FDFileInfo file_info;
	file_info.parent_path = DIR_PATH;

	std::vector<std::string> vec_dir;
	std::vector<FDFileInfo>  vec_file;
	while ( m_pathDir.GetFileName(file_info.file_name, file_info.file_type, file_info.file_size, file_info.chg_time) )
	{
		if ( base::BaseDir::DFT_DIR == file_info.file_type )
		{
			vec_dir.push_back(DIR_PATH+file_info.file_name);

			if ( m_dirVisible )
			{
				vec_file.push_back(file_info);
			}
		}
		else
		{
			vec_file.push_back(file_info);
		}
	}
	m_pathDir.Close();

	if ( m_recurse )
	{
		RecurseGetFileList(vec_dir, vec_file);
	}

	vec_file.swap(vec_fileinfo);
}

void FullDir::RecurseGetFileList(std::vector<std::string>& vec_dir, std::vector<FDFileInfo>& vec_file) throw(base::Exception)
{
	base::BaseDir            b_dir;
	std::string              str_path;
	std::string              str_error;
	FDFileInfo               file_info;
	std::vector<std::string> vec_subdir;

	const int VEC_SIZE = vec_dir.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		if ( !b_dir.SetPath(vec_dir[i]) )
		{
			throw base::Exception(ERR_CHKDLY_SETPATH_FAIL, "The file path \"%s\" is not ready! [FILE:%s, LINE:%d]", vec_dir[i].c_str(), __FILE__, __LINE__);
		}
		str_path = b_dir.GetPath();

		if ( !b_dir.Open(&str_error) )
		{
			throw base::Exception(ERR_CHKDLY_OPENDIR_FAIL, "Can not open the file path \"%s\": %s [FILE:%s, LINE:%d]", str_path.c_str(), str_error.c_str(), __FILE__, __LINE__);
		}

		file_info.parent_path = str_path;
		std::vector<std::string>().swap(vec_subdir);

		while ( b_dir.GetFileName(file_info.file_name, file_info.file_type, file_info.file_size, file_info.chg_time) )
		{
			if ( base::BaseDir::DFT_DIR == file_info.file_type )
			{
				vec_subdir.push_back(str_path+file_info.file_name);

				if ( m_dirVisible )
				{
					vec_file.push_back(file_info);
				}
			}
			else
			{
				vec_file.push_back(file_info);
			}
		}
		b_dir.Close();

		RecurseGetFileList(vec_subdir, vec_file);
	}
}

