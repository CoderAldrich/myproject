// WebRunThread.cpp : 实现文件
//

#include "stdafx.h"
#include "CommenWeb.h"
#include "WebRunThread.h"
#include "MainFrm.h"


IMPLEMENT_DYNCREATE(CWebRunThread, CWinThread)

BEGIN_MESSAGE_MAP(CWebRunThread, CWinThread)
	ON_THREAD_MESSAGE(WM_USER+1111,OnCWCreateView)
	ON_THREAD_MESSAGE(WM_USER+1112,OnCWViewReady)
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
	
	CW_CREATE_VIEW Info;
	Info.hEvent = hEvent;
	Info.ppWbControler = ppWbControl;

	m_ViewRecord.AddRecord(pMainFrame->m_pView,Info);

}

void CWebRunThread::OnCWViewReady(WPARAM wParam,LPARAM lParam)
{
	CW_CREATE_VIEW Info;
	BOOL bRes = m_ViewRecord.DelRecord((PVOID)wParam,&Info);
	if ( bRes )
	{
		if (Info.ppWbControler)
		{
			*(Info.ppWbControler) = dynamic_cast<IWBCoreControler *>((CIECoreView *)wParam);
		}

		SetEvent(Info.hEvent);
	}


}

// CWebRunThread 消息处理程序
