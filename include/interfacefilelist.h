#pragma once

#include <string>
#include <map>
#include <vector>
#include <set>
#include "exception.h"

namespace base
{
class Log;
}

struct FDFileInfo;
class OptionSet;
class InterfaceFile;
class Period;
struct OutputFileState;

class InterfaceFileList
{
public:
	typedef std::set<std::string>					SET_STR;
	typedef std::vector<InterfaceFile>				VEC_IFF;
	typedef std::map<int, VEC_IFF>					MAP_PSEQ_IFF;
	typedef std::map<std::string, OptionSet>		MAP_OPS;

public:
	InterfaceFileList(int path_size, const Period& period);
	~InterfaceFileList();

public:
	void ImportFile(const std::string& interface_file) throw(base::Exception);

	void ImportOptions(const std::vector<std::string>& vec_ops) throw(base::Exception);

	bool IsPathSeqValid(int seq) const;

	OptionSet* GetOptionSet(const std::string& op) throw(base::Exception);

	void GetChannels(SET_STR& s_chann) const { s_chann = m_setChannel; }

	void CompareFile(int path_seq, const std::vector<FDFileInfo>& vec_fi);

	void ExportFileState(std::vector<OutputFileState>& vec_ofs);

private:
	base::Log*       m_pLog;

private:
	int              m_pathSize;			// 路径个数
	const Period*    m_pPeriod;				// 账期时间
	MAP_OPS          m_mapOps;
	MAP_PSEQ_IFF     m_mapPSeqIff;
	SET_STR          m_setChannel;
};

