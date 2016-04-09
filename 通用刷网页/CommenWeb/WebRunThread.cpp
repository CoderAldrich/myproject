// WebRunThread.cpp : ʵ���ļ�
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
	// TODO: �ڴ�ִ���������߳�����
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


// CWebRunThread ��Ϣ�������
