#pragma once

#include <string>

// 接口文件状态
class IFFileState
{
public:
	// 文件状态
	enum IFFS_State
	{
		IFFSTATE_NORMAL      = 0,		// 正常
		IFFSTATE_MISSING     = 1,		// 缺失
		IFFSTATE_BLANK       = 2,		// 内容为空
		IFFSTATE_DELAY       = 3,		// 延迟
		IFFSTATE_DELAY_BLANK = 4,		// 延迟，内容为空
	};

public:
	IFFileState()
	:m_iffState(IFFSTATE_NORMAL)
	,m_stateMissing(false)
	,m_stateBlank(false)
	,m_stateDelay(false)
	,m_fileSize(0)
	{
	}

	~IFFileState() {}

public:
	// Getter
	bool GetStateMissing() const { return m_stateMissing; }
	bool GetStateBlank()   const { return m_stateBlank  ; }
	bool GetStateDelay()   const { return m_stateDelay  ; }

	// 获取文件的状态
	IFFS_State GetFileState() const { return m_iffState; }

	// 设置文件的状态
	void SetFileState(IFFS_State state)
	{
		m_iffState = state;

		switch ( m_iffState )
		{
		case IFFSTATE_NORMAL:			// 正常
			m_stateMissing = false;
			m_stateBlank   = false;
			m_stateDelay   = false;
			break;
		case IFFSTATE_MISSING:			// 缺失
			m_stateMissing = true;
			m_stateBlank   = false;
			m_stateDelay   = false;
			break;
		case IFFSTATE_BLANK:			// 内容为空
			m_stateMissing = false;
			m_stateBlank   = true;
			m_stateDelay   = false;
			break;
		case IFFSTATE_DELAY:			// 延迟
			m_stateMissing = false;
			m_stateBlank   = false;
			m_stateDelay   = true;
			break;
		case IFFSTATE_DELAY_BLANK:		// 延迟，内容为空
			m_stateMissing = false;
			m_stateBlank   = true;
			m_stateDelay   = true;
			break;
		}
	}

private:
	IFFS_State  m_iffState;				// 文件状态
	bool        m_stateMissing;			// 子状态：缺失
	bool        m_stateBlank;			// 子状态：内容为空
	bool        m_stateDelay;			// 子状态：延迟

public:
	int         m_fileSize;
	std::string m_chgtime;
};

