// WebRunThread.cpp : 实现文件
//

#include "stdafx.h"
#include "CommenWeb.h"
#include "WebRunThread.h"
#include "MainFrm.h"


IMPLEMENT_DYNCREATE(CWebRunThread, CWinThread)

BEGIN_MESSAGE_MAP(CWebRunThread, CWinThread)
	ON_THREAD_MESSAGE(WM_USER+1111,OnCWCreateView)
END_MESSAGE_MAP()

CWebRunThread::CWebRunThread()
{
}

CWebRunThread::~CWebRunThread()
{

}

BOOL CWebRunThread::InitInstance()
{
	CoInitialize(NULL);
	return TRUE;
}

int CWebRunThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

void CWebRunThread::OnCWCreateView(WPARAM wParam,LPARAM lParam)
{
	HANDLE hEvent = (HANDLE)wParam;
	IWBCoreControler **ppWbControl = (IWBCoreControler **)lParam;

	CMainFrame *pMainFrame = new CMainFrame;
	pMainFrame->CreateInstance();
	
	if (ppWbControl)
	{
		*ppWbControl = dynamic_cast<IWBCoreControler *>(pMainFrame->m_pView);
	}

	SetEvent(hEvent);
}


// CWebRunThread 消息处理程序
