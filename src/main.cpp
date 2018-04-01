#include <iostream>
#include <unistd.h>
#include "log.h"
#include "pubstr.h"
#include "simpletime.h"
#include "check_delay.h"

const char* g_version = "v2.00";

int main(int argc, char* argv[])
{
	if ( argc != 3 )
	{
		std::cerr << "[usage] " << argv[0] << " log_prefix configuration_file" << std::endl;
		return -1;
	}

	base::Log::SetLogFilePrefix(argv[1]);

	long long log_id = 0;
	base::PubStr::Str2LLong(base::SimpleTime::Now().Time17(), log_id);
	base::Log::SetLogID(log_id);

	base::AutoLogger aLog;
	base::Log* pLog = aLog.Get();

	try
	{
		pLog->SetPath(CheckDelay::GetCfgLogPath(argv[2]));
		pLog->Init();
		pLog->Output("%s, PID=[%d]", g_version, getpid());

		try
		{
			CheckDelay chk_delay;

			chk_delay.LoadConfig(argv[2]);
			chk_delay.Init();
			chk_delay.Run();
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

	return 0;
}

