#include <iostream>
#include <unistd.h>
#include "log.h"
#include "pubstr.h"
#include "checkdelay.h"

#ifdef TEST
#include "interfacefile.h"
#include "interfacefilelist.h"
#include "optionset.h"
#include "period.h"

void Test()
{
	Period prd;
	prd.Init("T - 3");

	// 1) YYYYMMDD, 例如 20180401
	// 2) YYYYMM, 例如 201804
	// 3) YYMMDD, 例如 180401
	// 4) YYMM, 例如 1804
	// 5) MMDD, 例如 0401
	std::cout << "YYYYMMDD: " << prd.GetDay_fmt("YYYYMMDD") << std::endl;
	std::cout << "YYYYMM  : " << prd.GetDay_fmt("YYYYMM") << std::endl;
	std::cout << "YYMMDD  : " << prd.GetDay_fmt("YYMMDD") << std::endl;
	std::cout << "YYMM    : " << prd.GetDay_fmt("YYMM") << std::endl;
	std::cout << "MMDD    : " << prd.GetDay_fmt("MMDD") << std::endl;
	std::cout << "YYDD    : " << prd.GetDay_fmt("YYDD") << std::endl;
}
#endif

const char* g_version = "Version v4.00 released. Compiled at " __TIME__ " on " __DATE__;

int main(int argc, char* argv[])
{
	if ( argc != 3 )
	{
		std::cerr << "[usage] " << argv[0] << " log_id configuration_file" << std::endl;
		return -1;
	}

	base::Log::SetLogFilePrefix("CHKDLY");

	long long log_id = 0;
	if ( !base::PubStr::Str2LLong(argv[1], log_id) )
	{
		std::cerr << "[ERROR] Invalid log ID: " << argv[1] << std::endl;
		return -1;
	}
	if ( !base::Log::SetLogID(log_id) )
	{
		std::cerr << "[ERROR] Set log ID failed !" << std::endl;
		return -1;
	}

	base::AutoLogger aLog;
	base::Log* pLog = aLog.Get();

	try
	{
		pLog->SetPath(CheckDelay::GetCfgLogPath(argv[2]));
		pLog->Init();
		pLog->Output("%s, PID=[%d]", g_version, getpid());

		try
		{
#ifdef TEST
			Test();
#else
			CheckDelay chk_delay;

			chk_delay.LoadConfig(argv[2]);
			chk_delay.Init();
			chk_delay.Run();
#endif
		}
		catch ( base::Exception& ex )
		{
			pLog->Output("[ERROR] %s, ERROR_CODE: %d", ex.What().c_str(), ex.ErrorCode());
			throw ex;
		}
	}
	catch ( base::Exception& ex )
	{
		std::cerr << "[ERROR] " << ex.What() << ", ERROR_CODE: " << ex.ErrorCode() << std::endl;
		return -1;
	}

	std::cout << argv[0] << " quit!" << std::endl;
	pLog->Output("%s quit!", argv[0]);
	return 0;
}

