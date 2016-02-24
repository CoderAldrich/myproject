#include "StdAfx.h"
#include "INotifyerInstance.h"
#include "WebBrowser.h"

INotifyerInstance::INotifyerInstance(IUIControler *pControler)
{
	m_pUIControler = pControler;
	m_CurrentPageID = 0;
}

INotifyerInstance::~INotifyerInstance(void)
{
	
}

bool INotifyerInstance::InsertPage(PAGEID nPageID,PAGEINFO *pPageInfo)
{
	if(pPageInfo)
	{
		m_mapPagesLocker.Lock();
		m_mapPages.insert(make_pair(nPageID,*pPageInfo));
		m_mapPagesLocker.UnLock();

		NotifySetCurrentPage(nPageID);
		return true;
	}
	return false;
}

bool INotifyerInstance::RemovePage(PAGEID nPageID)
{
	bool bRes = false;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR it = m_mapPages.find(nPageID);
	if (it!= m_mapPages.end())
	{
		m_mapPages.erase(it);
		bRes = true;
	}
	m_mapPagesLocker.UnLock();
	return bRes;
	
}

BOOL INotifyerInstance::InsertCloseingThread(HANDLE hThread)
{
	m_listCloseingThreadLocker.Lock();
	m_listCloseingThread.push_back( hThread );
	m_listCloseingThreadLocker.UnLock();
	return true;
}
HWND INotifyerInstance::PageIDToWnd(PAGEID nPageID)
{
	HWND hWnd = NULL;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR p = m_mapPages.find(nPageID);
	if (p!=m_mapPages.end())
	{
		hWnd = p->second.hWnd;
	}
	m_mapPagesLocker.UnLock();
	return hWnd;
}

BOOL INotifyerInstance::SetPageWnd(PAGEID nPageID,HWND hWnd)
{
	BOOL bRes = FALSE;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR p = m_mapPages.find(nPageID);
	if (p!=m_mapPages.end())
	{
		p->second.hWnd = hWnd;
		bRes = TRUE;
	}
	m_mapPagesLocker.UnLock();
	return bRes;
}

IWBCoreControler * INotifyerInstance::PageIDToCoreControl(PAGEID nPageID)
{
	IWBCoreControler *pControler = NULL;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR p = m_mapPages.find(nPageID);
	if ( p!=m_mapPages.end() )
	{
		pControler = p->second.pCoreControler;
	}
	m_mapPagesLocker.UnLock();
	return pControler;
}
CString INotifyerInstance::PageIDToUrl(PAGEID nPageID)
{
	CString strUrl;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR p = m_mapPages.find(nPageID);
	if (p!=m_mapPages.end())
	{
		strUrl = p->second.strUrl;
	}
	m_mapPagesLocker.UnLock();
	return strUrl;
}
CString INotifyerInstance::PageIDToTitle(PAGEID nPageID)
{
	CString strTitle;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR p = m_mapPages.find(nPageID);
	if (p!=m_mapPages.end())
	{
		strTitle = p->second.strTitle;
	}
	m_mapPagesLocker.UnLock();
	return strTitle;
}

bool INotifyerInstance::SetPageTitle(PAGEID nPageID,LPCTSTR pszNewTitle)
{
	bool bRes = false;
	CString strTitle;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR p = m_mapPages.find(nPageID);
	if (p!=m_mapPages.end())
	{
		p->second.strTitle = pszNewTitle;
		bRes = true;
	}
	m_mapPagesLocker.UnLock();
	return bRes;
	
}
bool INotifyerInstance::SetPageUrl(PAGEID nPageID,LPCTSTR pszNewUrl)
{
	bool bRes = false;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR p = m_mapPages.find(nPageID);
	if (p!=m_mapPages.end())
	{
		p->second.strUrl = pszNewUrl;
		bRes = true;
	}
	m_mapPagesLocker.UnLock();
	return bRes;
}
bool INotifyerInstance::SetPageCommandStatus(PAGEID nPageID,BOOL bCanBack,BOOL bCanForward)
{
	bool bRes=false;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR p = m_mapPages.find(nPageID);
	if (p!=m_mapPages.end())
	{
		p->second.bCanBack = bCanBack;
		p->second.bCanForward = bCanForward;
		bRes = true;
	}
	m_mapPagesLocker.UnLock();
	return bRes;
}
BOOL INotifyerInstance::GetPageCanBack(PAGEID nPageID)
{
	BOOL bCanBack = FALSE;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR p = m_mapPages.find(nPageID);
	if (p!=m_mapPages.end())
	{
		bCanBack = p->second.bCanBack;
	}
	m_mapPagesLocker.UnLock();
	return bCanBack;
}
BOOL INotifyerInstance::GetPageCanForward(PAGEID nPageID)
{
	BOOL bCanForward = FALSE;
	m_mapPagesLocker.Lock();
	PAGEMAPPTR p = m_mapPages.find(nPageID);
	if (p!=m_mapPages.end())
	{
		bCanForward = p->second.bCanForward;
	}
	m_mapPagesLocker.UnLock();
	return bCanForward;
}
bool INotifyerInstance::UpdatePageShow()
{
	CRect rcClient;
	m_pUIControler->ControlQueryClientRect(&rcClient);
	IWBCoreControler *pWbControl = PageIDToCoreControl(m_CurrentPageID);
	if(pWbControl)
	{
		pWbControl->ControlShowWindow(TRUE);
		pWbControl->ControlMoveWindow(rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height());
		pWbControl->ControlSetFocus();
	}

	//隐藏其他网页
 	m_mapPagesLocker.Lock();
 	for (PAGEMAPPTR it = m_mapPages.begin();it!= m_mapPages.end();it++)
 	{
 		pWbControl = it->second.pCoreControler;
 		
 		if( pWbControl && it->first != m_CurrentPageID)
 		{
 			pWbControl->ControlShowWindow(FALSE);
 		}
 	}
 	m_mapPagesLocker.UnLock();

	return 0;

}

unsigned long INotifyerInstance::NotifyGotoUrl( const wchar_t *pszTargetUrl , bool bNewWindow )
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
        pCoreControl->ControlGotoUrl(pszTargetUrl);
	}
	return 0;
}
unsigned long INotifyerInstance::NotifyGoHome( )
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		pCoreControl->ControlGotoUrl(theApp.strHomePage);
	}

	return 0;
}
unsigned long INotifyerInstance::NotifyGoBack(  )
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		pCoreControl->ControlGoBack();
	}

	return 0;
}
unsigned long INotifyerInstance::NotifyGoForward( )
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		pCoreControl->ControlGoForward();
	}
	return 0;
}
unsigned long INotifyerInstance::NotifyRefresh( )
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		pCoreControl->ControlRefresh();
	}
	return 0;
}
unsigned long INotifyerInstance::NotifyStopLoading()
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		pCoreControl->ControlStopLoading();
	}
	return 0;
}

unsigned long INotifyerInstance::NotifyUINewWindow(LPCTSTR pszUrl)
{
	NotifyNewWindow(NULL,theApp.strHomePage,NULL,FALSE);
	return 0;
}
unsigned long INotifyerInstance::NotifyPageClose( PAGEID nPageID )
{
	IWBCoreControler *pWBCoreControler = PageIDToCoreControl(nPageID);
	if(pWBCoreControler)
	{
		HWND hCoreWnd = NULL;
		pWBCoreControler->ControlQueryWnd(&hCoreWnd);
		HANDLE hPageThread = GetPropW(hCoreWnd,L"CorePageThread");
		
		ASSERT(hPageThread);
		
		DettachWBCoreFrameFrame(pWBCoreControler,m_pUIControler);

		if (pWBCoreControler)
		{
			pWBCoreControler->ControlClose();
		}

		InsertCloseingThread(hPageThread);

		size_t nPageCount = 0;
		m_mapPagesLocker.Lock();
		nPageCount = m_mapPages.size();
		m_mapPagesLocker.UnLock();
		
		if (nPageCount == 0)
		{
			HWND hFrameWnd = NULL;
			m_pUIControler->ControlQueryQueryMainFrame(&hFrameWnd);
			PostMessage(hFrameWnd,WM_CLOSE,0,0);
		}
	}

	return 0;
}
unsigned long INotifyerInstance::NotifyEditCut()
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		pCoreControl->ControlEditCut();
	}
	return 0;
}
unsigned long INotifyerInstance::NotifyEditCopy()
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		pCoreControl->ControlEditCopy();
	}
	return 0;
}
unsigned long INotifyerInstance::NotifyEditPaste()
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		pCoreControl->ControlEditPaste();
	}
	return 0;
}
unsigned long INotifyerInstance::NotifyEditSelectAll()
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		pCoreControl->ControlEditSelectAll();
	}
	return 0;
}
unsigned long INotifyerInstance::NotifyEditFind()
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		pCoreControl->ControlEditFind();
	}
	return 0;
}
unsigned long INotifyerInstance::NotifySearchText(LPCTSTR pszText)
{
	IWBCoreControler *pCoreControl = PageIDToCoreControl(m_CurrentPageID);
	if (pCoreControl)
	{
		CString strSearchUrl;

		if (strSearchUrl.GetLength() == 0 )
		{
			strSearchUrl = L"http://www.baidu.com/s?tn=00005079_pg&wd=";
			strSearchUrl += pszText;
		}
		
		pCoreControl->ControlGotoUrl(strSearchUrl);
	}
	return 0;
}
unsigned long INotifyerInstance::NotifyFrameClose(BOOL *pbCanClose)
{
	HWND hMainFrame = NULL;
	m_pUIControler->ControlQueryQueryMainFrame(&hMainFrame);
	ShowWindow(hMainFrame,SW_HIDE);


	BOOL bCanClose = FALSE;
	PAGEMAP tempPageMap;
	m_mapPagesLocker.Lock();

	if ( m_mapPages.size() == 0 )
	{
		bCanClose = TRUE;
	}
	else
	{
		bCanClose = FALSE;
		for (PAGEMAPPTR it = m_mapPages.begin();it!= m_mapPages.end();it++)
		{
			tempPageMap.insert(make_pair(it->first,it->second));
		}
	}

	m_mapPagesLocker.UnLock();

	if ( tempPageMap.size() > 0 )
	{
		for (PAGEMAPPTR ittemp = tempPageMap.begin();ittemp!= tempPageMap.end();ittemp++)
		{
			PAGEID nPageID = ittemp->first;
			ULONG bRes = NotifyPageClose( nPageID );
		}
	}


	
 	m_listCloseingThreadLocker.Lock();
 
	CLOSEING_THREAD tempCloseThread;
 	for(CLOSEING_THREAD_PTR it = m_listCloseingThread.begin();it!=m_listCloseingThread.end();it++)
 	{
 		HANDLE hPageThread = *it;

		DWORD dwExitCode = 0;
		BOOL bRes = GetExitCodeThread(hPageThread,&dwExitCode);

		if ( bRes && STILL_ACTIVE == dwExitCode )
		{
			tempCloseThread.push_back(hPageThread);
		}
 	}
	
	m_listCloseingThread.clear();

	if ( tempCloseThread.size() > 0 )
	{
#ifdef DEBUG
		CString strMsgOut;
		strMsgOut.Format(L"还有%d线程没有退出\n",tempCloseThread.size());
		OutputDebugStringW(strMsgOut);

#endif
		bCanClose = FALSE;

		for(CLOSEING_THREAD_PTR it = tempCloseThread.begin();it!=tempCloseThread.end();it++)
		{
			m_listCloseingThread.push_back(*it);
		}
	}

 	m_listCloseingThreadLocker.UnLock();

	if (pbCanClose)
	{
		*pbCanClose = bCanClose;
	}
	
	if ( FALSE == bCanClose )
	{
#ifdef DEBUG
		OutputDebugStringW(L"***********Frame线程不能退出********\n");
#endif
		Sleep(100);
		::PostMessage(hMainFrame,WM_CLOSE,0,0);
	}
#ifdef DEBUG
	else
	{
		OutputDebugStringW(L"***********Frame线程可以退出********\n");
	}
#endif
	return 0;
}
//only for ie8 or ie9
unsigned long INotifyerInstance::NotifyChangeTab(PAGEID nPageID,PAGEID nOldPageID)
{
	NotifySetCurrentPage(nPageID);

	//通知UI改变显示内容
	CString strUrl = PageIDToUrl(nPageID);
	m_pUIControler->ControlFrameAddrChange(strUrl);
	CString strTitle = PageIDToTitle(nPageID);
	m_pUIControler->ControlFrameTitleChange(strTitle);

	return 0;
}

unsigned long INotifyerInstance::NotifyShowSource()
{
	return 0;
}
unsigned long INotifyerInstance::NotifyShowAboutBrowser()
{
	return 0;
}
unsigned long INotifyerInstance::NotifySizeWindow(int nLeft,int nTop,int nWidht,int nHeight)
{
	IWBCoreControler *pWbControl = PageIDToCoreControl(m_CurrentPageID);
	pWbControl->ControlMoveWindow(nLeft,nTop,nWidht,nHeight);

	return 0;
}
unsigned long INotifyerInstance::NotifyDragTab(PAGEID nPageID)
{
	return 0;
}
//浏览器内核通知函数
unsigned long INotifyerInstance::NotifyNewWindow(PVOID *ppPageRef,LPCTSTR pszUrl,BOOL *bCancel,BOOL bNewFrame,LONG * nNewPageID/*=NULL*/)
{
	CRect rcClient;
	HWND hMainFrame = NULL;
	m_pUIControler->ControlQueryClientRect(&rcClient);
	m_pUIControler->ControlQueryQueryMainFrame(&hMainFrame);

	if(theApp.m_nBaseStyle != 6 && !bNewFrame && theApp.m_bMutiTab )
	{
		NewBrowserInstance(m_pUIControler,ppPageRef,pszUrl,nNewPageID);
	}
	else
	{
		NewBrowserInstance(NULL,ppPageRef,pszUrl,nNewPageID);
	}

	return 0;
}
unsigned long INotifyerInstance::NotifyBeforeMainNavigate(PAGEID nPageID,LPCTSTR pszUrl,BOOL *bCancel)
{
    return 0;
}
unsigned long INotifyerInstance::NotifyBeforeSubNavigate(PAGEID nPageID,LPCTSTR pszUrl,BOOL *bCancel)
{
	return true;
}
unsigned long INotifyerInstance::NotifyMainDocumentComplete(PAGEID nPageID,LPCTSTR pszUrl)
{

	return 0;
}
unsigned long INotifyerInstance::NotifyTitleChange(PAGEID nPageID,LPCTSTR pszNewTitle)
{
	SetPageTitle(nPageID,pszNewTitle);
	if(m_pUIControler)
	{
		m_pUIControler->ControlPageTitleChange(nPageID,pszNewTitle);

		//只有当获取焦点的页面Title发生变化时才通知主窗口修改标题
		if(nPageID == m_CurrentPageID)
		{
			m_pUIControler->ControlFrameTitleChange(pszNewTitle);
		}
	}

	return 0;
}
unsigned long INotifyerInstance::NotifyUrlChange(PAGEID nPageID,LPCTSTR pszNewUrl)
{
	SetPageUrl(nPageID,pszNewUrl);
	if(m_pUIControler)
	{
		m_pUIControler->ControlPageUrlChange(nPageID,pszNewUrl);
		m_pUIControler->ControlFrameAddrChange(pszNewUrl);
	}
	return 0;
}

unsigned long INotifyerInstance::NotifyStatusCommand(PAGEID nPageID,BOOL bCanBack,BOOL bCanForward)
{
	SetPageCommandStatus(nPageID,bCanBack,bCanForward);

	if (nPageID == m_CurrentPageID)
	{
		m_pUIControler->ControlBackForward(bCanBack,bCanForward);
	}

	return 0;
}
unsigned long INotifyerInstance::NotifyStatusTextChange(PAGEID nPageID,LPCTSTR pszNewStatusText)
{
	if (nPageID == m_CurrentPageID)
	{
		m_pUIControler->ControlFrameStatusChage(pszNewStatusText);
	}
	return 0;
}
unsigned long INotifyerInstance::NotifySetCurrentPage(PAGEID nPageID)
{
	m_CurrentPageID = nPageID;
	m_pUIControler->ControlFrameAddrChange(PageIDToUrl(m_CurrentPageID));
	m_pUIControler->ControlFrameTitleChange(PageIDToTitle(m_CurrentPageID));
	m_pUIControler->ControlBackForward(GetPageCanBack(m_CurrentPageID),GetPageCanForward(m_CurrentPageID));
	m_pUIControler->ControlSetFocusTab(m_CurrentPageID);

	UpdatePageShow();
	return  0;
}
unsigned long INotifyerInstance::NotifyCloseThis(PAGEID nPageID)
{
	NotifyPageClose(nPageID);
	return 0;
}