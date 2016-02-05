// NoParentThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WebBrowser.h"
#include "NoParentThread.h"

#include "NoParentFrameWnd.h"

// CNoParentThread

IMPLEMENT_DYNCREATE(CNoParentThread, CWinThread)

CNoParentThread::CNoParentThread()
{
}

CNoParentThread::~CNoParentThread()
{
}

BOOL CNoParentThread::InitInstance()
{
 	CNoParentFrameWnd *pMainFrame = new CNoParentFrameWnd();
 	pMainFrame->Create(NULL,NULL,WS_OVERLAPPEDWINDOW);
 	
 	pMainFrame->ShowWindow(SW_HIDE);
 	theApp.m_pMainWnd = m_pMainWnd = pMainFrame;
	
	return TRUE;
}

int CNoParentThread::ExitInstance()
{
	//delete m_pMainWnd;
	// TODO: �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CNoParentThread, CWinThread)
END_MESSAGE_MAP()


// CNoParentThread ��Ϣ�������
