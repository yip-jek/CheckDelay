#include "checkdb2.h"
#include "errorcode.h"
#include "log.h"
#include "pubstr.h"
#include "simpletime.h"
#include "outputfilestate.h"

CheckDB2::CheckDB2(const std::string& db_name, const std::string& usr, const std::string& pw)
:BaseDB2(db_name, usr, pw)
{
}

CheckDB2::~CheckDB2()
{
}

void CheckDB2::SetOutputTab(const std::string& output_tab)
{
	m_outputTab = output_tab;
}

void CheckDB2::DeleteOldData(const std::string& date_time) throw(base::Exception)
{
	XDBO2::CRecordset rs(&m_CDB);
	rs.EnableWarning(true);

	std::string sql = "DELETE FROM " + m_outputTab + " WHERE DATE_TIME = ?";
	m_pLog->Output("[DB2] Delete old data: DATE_TIME=[%s]", date_time.c_str());

	try
	{
		rs.Prepare(sql.c_str(), XDBO2::CRecordset::forwardOnly);
		rs.Parameter(1) = date_time.c_str();
		m_pLog->Output("[DB2] Execute delete sql: %s", sql.c_str());

		Begin();
		rs.Execute();
		Commit();
	}
	catch ( const XDBO2::CDBException& ex )
	{
		if ( IsNoRowDeleted(ex.what()) )
		{
			m_pLog->Output("[DB2] No row was found for DELETE !");
		}
		else
		{
			throw base::Exception(ERR_DB2_DELOLDDATA_FAIL, "[DB2] Delete from table '%s' failed! [DATE_TIME=%s] [CDBException] %s [FILE:%s, LINE:%d]", m_outputTab.c_str(), date_time.c_str(), ex.what(), __FILE__, __LINE__);
		}
	}
}

void CheckDB2::MergeResultData(const std::vector<OutputFileState>& vec_ofs) throw(base::Exception)
{
	XDBO2::CRecordset rs(&m_CDB);
	rs.EnableWarning(true);

	// 生成时间为当前时间
	std::string modify_time = base::SimpleTime::Now().TimeStamp();

	// Select SQL
	std::string sql_sel = "SELECT COUNT(0) FROM " + m_outputTab + " WHERE DATE_TIME = ? and FILE_NAME = ?";

	// Insert SQL
	std::string sql_ins = "INSERT INTO " + m_outputTab + "(DATE_TIME, CHANNEL, FILE_NAME, ";
	sql_ins += "FILE_SIZE, FILE_CHGTIME, STATE_MISSING, STATE_BLANK, STATE_DELAY, ";
	sql_ins += "STATE_DESC, MODIFY_TIME) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";

	// Update SQL
	std::string sql_upd = "UPDATE " + m_outputTab + " SET CHANNEL = ?, FILE_SIZE = ?, ";
	sql_upd += "FILE_CHGTIME = ?, STATE_MISSING = ?, STATE_BLANK = ?, STATE_DELAY = ?, ";
	sql_upd += "STATE_DESC = ?, MODIFY_TIME = ? WHERE DATE_TIME = ? AND FILE_NAME = ?";

	m_pLog->Output("[DB2] Merge result data: [%lu]", vec_ofs.size());

	try
	{
		int sel_count = 0;
		int ins_count = 0;
		int upd_count = 0;
		int index     = 0;

		const int VEC_SIZE = vec_ofs.size();
		for ( int i = 0; i < VEC_SIZE; ++i )
		{
			const OutputFileState& ref_ofs = vec_ofs[i];

			rs.Prepare(sql_sel.c_str(), XDBO2::CRecordset::forwardOnly);
			rs.Parameter(1) = ref_ofs.date_time.c_str();
			rs.Parameter(2) = ref_ofs.file_name.c_str();
			rs.Execute();

			while ( !rs.IsEOF() )
			{
				sel_count = (int)rs[1];

				rs.MoveNext();
			}
			rs.Close();

			if ( sel_count > 0 )	// 已存在，Update
			{
				rs.Prepare(sql_upd.c_str(), XDBO2::CRecordset::forwardOnly);

				index = 1;
				rs.Parameter(index++) = ref_ofs.channel.c_str();
				rs.Parameter(index++) = ref_ofs.file_size;
				rs.Parameter(index++) = ref_ofs.file_chgtime.c_str();
				rs.Parameter(index++) = (ref_ofs.state_missing ? "1" : "0");
				rs.Parameter(index++) = (ref_ofs.state_blank   ? "1" : "0");
				rs.Parameter(index++) = (ref_ofs.state_delay   ? "1" : "0");
				rs.Parameter(index++) = ref_ofs.state_desc.c_str();
				rs.Parameter(index++) = modify_time.c_str();
				rs.Parameter(index++) = ref_ofs.date_time.c_str();
				rs.Parameter(index++) = ref_ofs.file_name.c_str();
				rs.Execute();

				++upd_count;
				Commit();
				rs.Close();
			}
			else	// 不存在，Insert
			{
				rs.Prepare(sql_ins.c_str(), XDBO2::CRecordset::forwardOnly);

				index = 1;
				rs.Parameter(index++) = ref_ofs.date_time.c_str();
				rs.Parameter(index++) = ref_ofs.channel.c_str();
				rs.Parameter(index++) = ref_ofs.file_name.c_str();
				rs.Parameter(index++) = ref_ofs.file_size;
				rs.Parameter(index++) = ref_ofs.file_chgtime.c_str();
				rs.Parameter(index++) = (ref_ofs.state_missing ? "1" : "0");
				rs.Parameter(index++) = (ref_ofs.state_blank   ? "1" : "0");
				rs.Parameter(index++) = (ref_ofs.state_delay   ? "1" : "0");
				rs.Parameter(index++) = ref_ofs.state_desc.c_str();
				rs.Parameter(index++) = modify_time.c_str();
				rs.Execute();

				++ins_count;
				Commit();
				rs.Close();
			}
		}

		m_pLog->Output("[DB2] SQL: INSERT=[%d], UPDATE=[%d]", ins_count, upd_count);
	}
	catch ( const XDBO2::CDBException& ex )
	{
		throw base::Exception(ERR_DB2_MERGEDATA_FAIL, "[DB2] Merge result data to table '%s' failed! [CDBException] %s [FILE:%s, LINE:%d]", m_outputTab.c_str(), ex.what(), __FILE__, __LINE__);
	}
}

bool CheckDB2::IsNoRowDeleted(const std::string& exception)
{
	const std::string UPPER_EX = base::PubStr::UpperB(exception);

	return (UPPER_EX.find("NO ROW WAS FOUND") != std::string::npos 
		&& UPPER_EX.find("SQLSTATE=02000") != std::string::npos);
}

