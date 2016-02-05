// PageThread.cpp : 实现文件
//

#include "stdafx.h"
#include "WebBrowser.h"
#include "PageThread.h"

//#ifdef WIN64
//#include "IE32View.h"
//#endif
// CPageThread

UINT nMsgNewGetHtml = ::RegisterWindowMessage( _T("WM_DESTORY_EX") ); 

IMPLEMENT_DYNCREATE(CPageThread, CStrongWinThread)

CPageThread::CPageThread()
{
    nNewPageID = NULL;
	pWBCoreNotifyer = NULL;
	ppWBCoreControler = NULL;
	rcClient.SetRect(0,0,800,600);
	strInitUrl = TEXT("");
	m_dwCookie = 0;
	//pStream = NULL;
	hEvent = NULL;
	pIEView = NULL;
}

CPageThread::~CPageThread()
{
}


BOOL CPageThread::InitInstance()
{
    // 初始化 OLE 库
    if (!AfxOleInit())
    {
    	AfxMessageBox(IDP_OLE_INIT_FAILED);
    	return FALSE;
    }
    //CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY|COINIT_APARTMENTTHREADED);
    //OleInitialize(NULL);

	//////////////////////////////////////////////////////////////////////////
	pIEView = new CIECoreView(pWBCoreNotifyer);
	if(pIEView)
	{
		if(pIEView->Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcClient,theApp.m_pMainWnd/*CWnd::FromHandle(GetDesktopWindow())*/,0))
		{
			m_pMainWnd = pIEView;

			pIEView->SetPageID((PAGEID)(pIEView->m_hWnd));
			IWebBrowser2 *pWb2 = NULL;
            pIEView->GetApplication()->QueryInterface(IID_IWebBrowser2,(void **)&pWb2);
            if(pWb2)
            {
                pWb2->put_RegisterAsBrowser(VARIANT_FALSE);
            }

			theApp.GetGITPtr()->RegisterInterfaceInGlobal(pIEView->GetApplication(),IID_IDispatch,&pIEView->m_dwCookie);
			m_dwCookie = pIEView->m_dwCookie;

		}
	}

	if(ppWBCoreControler)
	{
		*ppWBCoreControler = dynamic_cast< IWBCoreControler* >(pIEView);
	}

	SetEvent(hEvent);

	return TRUE;
}

int CPageThread::ExitInstance()
{

	//SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPageThread, CStrongWinThread)
END_MESSAGE_MAP()


// CPageThread 消息处理程序
