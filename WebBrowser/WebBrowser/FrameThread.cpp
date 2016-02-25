// FrameThread.cpp : 实现文件
//

#include "stdafx.h"
#include "WebBrowser.h"
#include "FrameThread.h"
#include ".\UI\MainFrm.h"

// CFrameThread

IMPLEMENT_DYNCREATE(CFrameThread, CStrongWinThread)

CFrameThread::CFrameThread()
{
	pWnd = NULL;
	ppUIControler = NULL;
	pUINotifyer = NULL;
}

CFrameThread::~CFrameThread()
{
	theApp.DecreaseFrameCount();
}

BOOL CFrameThread::InitInstance()
{
    // 初始化 OLE 库
    //if (!AfxOleInit())
    //{
    //    AfxMessageBox(IDP_OLE_INIT_FAILED);
    //    return FALSE;
    //}
    //CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY|COINIT_MULTITHREADED);
    OleInitialize(NULL);

	static int   nWidth = 800;
	static int   nHeight = 600;

	static int   nFullWidth=GetSystemMetrics(SM_CXSCREEN); 
	static int   nFullHeight=GetSystemMetrics(SM_CYSCREEN); 
	int   nLeft = (nFullWidth-nWidth)/2;
	int   nTop = (nFullHeight-nHeight)/2;

	CWnd *pMainWnd = new CMainFrame(m_bMutiTab,m_bMenuBar,m_bToolBar,m_bCommandBar);

	((CMainFrame *)pMainWnd)->CreateEx(WS_EX_WINDOWEDGE,NULL,NULL,WS_OVERLAPPEDWINDOW,CRect( nLeft , nTop , nLeft + nWidth,nTop + nHeight),NULL,0);


    CString szSize;
    szSize = AfxGetApp()->GetProfileString(_T("settting"),_T("pos"),szSize);
    if (szSize.IsEmpty())
    {
        pMainWnd->CenterWindow();
        pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
        pMainWnd->UpdateWindow();
    }
    else
    {
        WINDOWPLACEMENT place = {0};
        place.length = sizeof(WINDOWPLACEMENT);
        sscanf(CStringA(szSize),"%u,%u,%d,%d,%d,%d,%d,%d,%d,%d",
            &place.flags,
            &place.showCmd,
            &place.ptMinPosition.x,
            &place.ptMinPosition.y,
            &place.ptMaxPosition.x,
            &place.ptMaxPosition.y,
            &place.rcNormalPosition.left,
            &place.rcNormalPosition.right,
            &place.rcNormalPosition.top,
            &place.rcNormalPosition.bottom);
        if(place.showCmd == SW_SHOWMINIMIZED)
        {
            place.showCmd = SW_SHOWNORMAL;
        }
        if (!SetWindowPlacement(pMainWnd->m_hWnd,&place))
        {
            pMainWnd->CenterWindow();
            pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
            pMainWnd->UpdateWindow();
        }
    }
	//pMainWnd->UpdateWindow();

	if(pWnd)
	{
		*pWnd = pMainWnd->m_hWnd;
	}
	IUIControler *pUIControler = dynamic_cast<IUIControler *>(pMainWnd);

	pUIControler->SetNofiyerPoint(pUINotifyer);
	
	if (ppUIControler)
	{
		*ppUIControler = pUIControler;
	}

	m_pMainWnd = pMainWnd;
	
	if(pMainWnd)
	{
		pMainWnd->SetForegroundWindow();
	}
	

	SetEvent(hEvent);

	return TRUE;
}

int CFrameThread::ExitInstance()
{
    OleUninitialize();
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CFrameThread, CStrongWinThread)
END_MESSAGE_MAP()

