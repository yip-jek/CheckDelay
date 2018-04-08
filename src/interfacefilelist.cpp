#include "interfacefilelist.h"
#include "basefile.h"
#include "pubstr.h"
#include "log.h"
#include "errorcode.h"
#include "fulldir.h"
#include "interfacefile.h"
#include "optionset.h"
#include "checkdelay.h"
#include "outputfilestate.h"

InterfaceFileList::InterfaceFileList(int path_size, const Period& period)
:m_pLog(base::Log::Instance())
,m_pathSize(path_size)
,m_pPeriod(&period)
{
}

InterfaceFileList::~InterfaceFileList()
{
	base::Log::Release();
}

void InterfaceFileList::ImportFile(const std::string& interface_file) throw(base::Exception)
{
	base::BaseFile bf_iff;
	if ( !bf_iff.Open(interface_file) )
	{
		throw base::Exception(ERR_CHKDLY_IFFLIST_FAIL, "无法打开接口文件清单: [%s] [FILE:%s, LINE:%d]", interface_file.c_str(), __FILE__, __LINE__);
	}

	if ( !bf_iff.ReadyToRead() )
	{
		throw base::Exception(ERR_CHKDLY_IFFLIST_FAIL, "无法读取接口文件清单: [%s] [FILE:%s, LINE:%d]", interface_file.c_str(), __FILE__, __LINE__);
	}

	unsigned int  count = 0;
	size_t        pos   = 0;
	std::string   line;
	InterfaceFile iff(*this, *m_pPeriod);

	m_mapPSeqIff.clear();
	m_setChannel.clear();

	while ( bf_iff.Read(line) )
	{
		++count;

		// 忽略注释
		pos = line.find('#');
		if ( pos != std::string::npos )
		{
			line.erase(pos);
		}

		base::PubStr::Trim(line);

		// 忽略空行
		if ( line.empty() )
		{
			continue;
		}

		iff.Init(line);
		m_setChannel.insert(iff.GetChannel());
		m_mapPSeqIff[iff.GetPathSeq()].push_back(iff);
	}

	m_pLog->Output("[INTERFACE_FILE_LIST] Read the interface_file_list line(s): %u", count);
	m_pLog->Output("[INTERFACE_FILE_LIST] Import interface_file(s) size: %lu", m_mapPSeqIff.size());
}

void InterfaceFileList::ImportOptions(const std::vector<std::string>& vec_ops) throw(base::Exception)
{
	m_mapOps.clear();
	std::string op_name;

	const int VEC_SIZE = vec_ops.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		base::PubStr::SetFormatString(op_name, "%s_%d", CheckDelay::S_OPTION_SET, i+1);

		OptionSet& ref_ops = m_mapOps[op_name];
		ref_ops.Init(vec_ops[i]);
		ref_ops.SetOption(op_name);
	}

	m_pLog->Output("[INTERFACE_FILE_LIST] Import option(s) size: %lu", m_mapOps.size());
}

bool InterfaceFileList::IsPathSeqValid(int seq) const
{
	return (seq >= 1 && seq <= m_pathSize);
}

OptionSet* InterfaceFileList::GetOptionSet(const std::string& op) throw(base::Exception)
{
	std::string       s_op  = op;
	int               index = 0;
	MAP_OPS::iterator m_it;

	OptionSet::TryComplexSubstitute(op, s_op, index);

	if ( (m_it = m_mapOps.find(s_op)) != m_mapOps.end() )
	{
		return &(m_it->second);
	}
	else
	{
		throw base::Exception(ERR_CHKDLY_IFFLIST_FAIL, "找不到对应的 OptionSet: [%s] [FILE:%s, LINE:%d]", s_op.c_str(), __FILE__, __LINE__);
	}
}

void InterfaceFileList::CompareFile(int path_seq, const std::vector<FDFileInfo>& vec_fi)
{
	MAP_PSEQ_IFF::iterator m_it = m_mapPSeqIff.find(path_seq);
	if ( m_mapPSeqIff.end() == m_it )	// 没有对应路径下需要监控的文件
	{
		m_pLog->Output("<WARNING> [INTERFACE_FILE_LIST] 没有对应路径下需要监控的文件: PATH_SEQ=[%d]", path_seq);
		return;
	}

	VEC_IFF& ref_vec_iff = m_it->second;
	const int VEC_RVI_SIZE = ref_vec_iff.size();

	const int VEC_SIZE = vec_fi.size();
	for ( int i = 0; i < VEC_SIZE; ++i )
	{
		const FDFileInfo& ref_fi = vec_fi[i];

		for ( int j = 0; j < VEC_RVI_SIZE; ++j )
		{
			InterfaceFile& ref_iff = ref_vec_iff[j];

			ref_iff.UpdateFileState(ref_fi);
		}
	}
}

void InterfaceFileList::ExportFileState(std::vector<OutputFileState>& vec_ofs)
{
}

