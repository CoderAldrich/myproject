#include "stdafx.h"
#include "WebRunThread.h"
#include "IWBCoreControler.h"
#include "IECoreView.h"
#include "WndProcHook.h"
#include "BrowserHelpFun.h"

static DWORD g_dwThreadId = 0;
BOOL g_bPhoneMode = FALSE;
VOID WINAPI CWInit( BOOL bPhoneMode , LPCWSTR pszUserAgent )
{
	CoInitialize(NULL);

	g_bPhoneMode = bPhoneMode;
	if ( bPhoneMode && pszUserAgent )
	{
		CIECoreView::m_strUserAgent = pszUserAgent;
	}

	BeginShieldHtmlMsg();
	RegisterBrowserEmulationMode(TRUE);
	BrowserFix();

	SetPriorityClass(GetCurrentProcess(),IDLE_PRIORITY_CLASS);

	CWinThread *pWinThread = AfxBeginThread(RUNTIME_CLASS(CWebRunThread));
	g_dwThreadId = pWinThread->m_nThreadID;

	//等待线程进入消息循环
	while( FALSE == PostThreadMessageW(g_dwThreadId,WM_NULL,NULL,NULL) ) Sleep(100);
}

IWBCoreControler * WINAPI CWCreateView()
{
	IWBCoreControler *pWbControl = NULL;
	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	BOOL bRes = PostThreadMessageW(g_dwThreadId,WM_USER+1111,(WPARAM)hEvent,(LPARAM)&pWbControl);
	WaitForSingleObject(hEvent,INFINITE);
	CloseHandle(hEvent);
	return pWbControl;
}

