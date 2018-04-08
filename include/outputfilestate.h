#pragma once

#include <string>
#include "iffilestate.h"

struct OutputFileState
{
public:
	OutputFileState()
	:file_size(0)
	,iff_state(IFFileState::IFFSTATE_NORMAL)
	,state_missing(false)
	,state_blank(false)
	,state_delay(false)
	{}

public:
	std::string             date_time;				// 账期：YYYYMMDD
	std::string             channel;				// 渠道
	std::string             file_name;				// 文件名
	int                     file_size;				// 文件大小
	std::string             file_chgtime;			// 文件修改时间：YYYY-MM-DD HH24:MI:SS
	IFFileState::IFFS_State iff_state;				// 状态
	bool                    state_missing;			// 状态：缺失，0 或 1
	bool                    state_blank;			// 状态：内容为空，0 或 1
	bool                    state_delay;			// 状态：延迟，0 或 1
	std::string             state_desc;				// 状态描述
};

