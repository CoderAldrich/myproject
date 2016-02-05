// StrongWinThread.cpp : 实现文件
//

#include "stdafx.h"
#include "WebBrowser.h"
#include "StrongWinThread.h"


// CStrongWinThread

IMPLEMENT_DYNCREATE(CStrongWinThread, CWinThread)

CStrongWinThread::CStrongWinThread()
{
}

CStrongWinThread::~CStrongWinThread()
{
}

BOOL CStrongWinThread::PumpMessage()
{
	_AFX_THREAD_STATE *pState = NULL;
#ifndef DEBUG
	__try
	{
#endif
	pState = AfxGetThreadState();
#ifndef DEBUG
	}
	__except(1)
	{
		return FALSE;
	}
#endif
	if (!::GetMessage(&(pState->m_msgCur), NULL, NULL, NULL))
	{
#ifdef _DEBUG
		TRACE(traceAppMsg, 1, "CWinThread::PumpMessage - Received WM_QUIT.\n");
		pState->m_nDisablePumpCount++; // application must die
#endif
		// Note: prevents calling message loop things in 'ExitInstance'
		// will never be decremented
		return FALSE;
	}

#ifdef _DEBUG
	if (pState->m_nDisablePumpCount != 0)
	{
		TRACE(traceAppMsg, 0, "Error: CWinThread::PumpMessage called when not permitted.\n");
		ASSERT(FALSE);
	}
#endif

	// process this message

#ifndef DEBUG
	__try
	{
#endif
	if (pState->m_msgCur.message != 0x036A && !AfxPreTranslateMessage(&(pState->m_msgCur)))
	{
		::TranslateMessage(&(pState->m_msgCur));
		::DispatchMessage(&(pState->m_msgCur));
	}
#ifndef DEBUG
	}
	__except(1)
	{
		
	}
#endif
	return TRUE;
}

BEGIN_MESSAGE_MAP(CStrongWinThread, CWinThread)
END_MESSAGE_MAP()


// CStrongWinThread 消息处理程序
