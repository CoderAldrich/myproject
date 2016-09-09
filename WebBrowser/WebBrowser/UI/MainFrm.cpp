
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
//#include "WebBrowser.h"

#include "MainFrm.h"
#include "..\resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL              g_UIMutiTabShow = TRUE;
BOOL              g_UIMenuBarShow = TRUE;
BOOL              g_UIToolBarShow = TRUE;
BOOL              g_UICommandBarShow = TRUE;


IMPLEMENT_DYNCREATE(CMainFrame , CFrameWnd)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

//extern IUIControler *g_pControler;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_NOTHING_ENABLE,OnDoNothing)
	ON_COMMAND_RANGE(ID_UI_BEGIN,ID_UI_END,OnUICommand)
	ON_MESSAGE(WM_TAB_SEL_CHANGE,OnTabSelChange)
	ON_MESSAGE(WM_KEY_CHANGE_TAB,OnKeyTabChange)
	ON_MESSAGE(WM_TAB_CLOSE,OnTabClose)
	ON_MESSAGE(WM_TAB_DRAG,OnDragTab)
	ON_UPDATE_COMMAND_UI(ID_GO_BACK,OnUpdateBackButton)
	ON_UPDATE_COMMAND_UI(ID_GO_FORWARD,OnUpdateForwardButton)
	ON_WM_CLOSE()
	ON_WM_NCACTIVATE()

	ON_MESSAGE(WM_ASYNC_URL_CHANGE,OnAsyncUrlChange)
	ON_MESSAGE(WM_ASYNC_TITLE_CHANGE,OnAsyncTitleChange)
	ON_MESSAGE(WM_ASYNC_STATUS_TEXT_CHANGE,OnAsyncStatusTextChange)
	ON_MESSAGE(WM_ASYNC_FRAME_ADDR_CHANGE,OnAsyncFrameAddrChange)
	ON_MESSAGE(WM_ASYNC_FRAME_TITLE_CHANGE,OnAsyncFrameTitleChange)
	ON_MESSAGE(WM_ASYNC_ADD_TAB,OnAsyncAddTab)
	ON_MESSAGE(WM_ASYNC_DEL_TAB,OnAsyncDelTab)
	ON_MESSAGE(WM_ASYNC_SET_FOCUS_TAB,OnAsyncSetFocusTab)
	ON_COMMAND(ID_PRINT_WEB_VIEW, &CMainFrame::OnPrintWebView)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,           // 状态行指示器
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,           // 状态行指示器
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame(BOOL bMutiTab,BOOL bMenuBar,BOOL bToolBar,BOOL bCommandBar)
{
	g_UIMutiTabShow = bMutiTab;
	g_UIMenuBarShow = bMenuBar;
	g_UIToolBarShow = bToolBar;
	g_UICommandBarShow = bCommandBar;

	m_pCurAddr = NULL;
	m_pUINotifyer = NULL;
	bCanBack = FALSE;
	bCanForward = FALSE;
	bFirstErase = TRUE;
}
CMainFrame::CMainFrame()
{
	g_UIMutiTabShow = TRUE;
	g_UIMenuBarShow = TRUE;
	g_UIToolBarShow = TRUE;
	g_UICommandBarShow = TRUE;

	m_pCurAddr = NULL;
	m_pUINotifyer = NULL;
	bCanBack = FALSE;
	bCanForward = FALSE;
	bFirstErase = TRUE;
}
CMainFrame::~CMainFrame()
{
}

CView* CMainFrame::GetActiveView() const
{
	return NULL;
}
void CMainFrame::SetActiveView(CView* pViewNew, BOOL bNotify)
{
	int a=0;
}
CDocument* CMainFrame::GetActiveDocument()
{
	return NULL;
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	HICON hMainIcon = LoadIconW(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hMainIcon,TRUE);
	SetIcon(hMainIcon,FALSE);

	m_wndRebar.Create(this);

	//////////////////////////////////////////////////////////////////////////
	//公用UI

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	for (int i=1;i<m_wndStatusBar.GetCount();i++)
	{	
		m_wndStatusBar.SetPaneInfo(i,m_wndStatusBar.GetItemID(i),m_wndStatusBar.GetPaneStyle(i),25);
	}

	const int nInternet = 6;
	m_wndStatusBar.SetPaneInfo(nInternet,m_wndStatusBar.GetItemID(nInternet),m_wndStatusBar.GetPaneStyle(nInternet),160);
	m_wndStatusBar.SetPaneInfo(8,m_wndStatusBar.GetItemID(8),m_wndStatusBar.GetPaneStyle(8),78);

	m_wndStatusBar.GetStatusBarCtrl().SetText(TEXT("Internet"),nInternet,0);
	HICON hStatusIco = (HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_STATUS_STATIC),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
	m_wndStatusBar.GetStatusBarCtrl().SetIcon(nInternet,hStatusIco);

	m_IEMenuBar.CreateEx(this,
		TBSTYLE_LIST|TBSTYLE_FLAT|TBSTYLE_TRANSPARENT,WS_CHILD | WS_VISIBLE | CBRS_TOOLTIPS |TBSTYLE_TRANSPARENT
		);
	m_IEMenuBar.LoadMenu(IDR_MAINFRAME);
	m_IEMenuBar.ModifyStyle(CBRS_BORDER_TOP,0,SWP_FRAMECHANGED);



	//////////////////////////////////////////////////////////////////////////
	//IE8UI
	m_wndNaviBar.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,CRect(0,0,1000,34),this ,0);
	if (g_UIMutiTabShow)
	{
		m_wndTabBar.Create(NULL,NULL,WS_VISIBLE|WS_CHILD|TCS_FIXEDWIDTH|WS_CLIPCHILDREN,CRect(0,0,30,30),this,123);
	}
	m_pCurAddr = (CComboBox *)m_wndNaviBar.GetAddrBarPtr();

	m_wndRebar.AddBar(&m_wndNaviBar,NULL,NULL,RBBS_NOGRIPPER);

	if (g_UIMutiTabShow)
	{
		m_wndRebar.AddBar(&m_wndTabBar,NULL,NULL,RBBS_NOGRIPPER|RBBS_BREAK);
	}

	m_wndRebar.AddBar(&m_IEMenuBar,NULL,NULL,RBBS_NOGRIPPER|RBBS_BREAK);


	if ( FALSE == g_UIMenuBarShow )
	{
		m_IEMenuBar.ShowWindow(SW_HIDE);
	}
	//永久去除MenuBar
	//m_IEMenuBar.ShowWindow(SW_HIDE);

	return 0;
}
LRESULT CMainFrame::OnTabSelChange( WPARAM wParam,LPARAM lParam )
{
	PAGEID nPageID = (PAGEID)wParam;
	if(m_pUINotifyer)
	{
		m_pUINotifyer->NotifyChangeTab(nPageID,0);
	}

	return 0;
}
LRESULT CMainFrame::OnKeyTabChange(WPARAM wParam,LPARAM lParam )
{
	if (g_UIMutiTabShow)
	{
		CIETabBar *pTabBar = m_wndTabBar.GetTabCtrl();
		if (pTabBar)
		{
			pTabBar->SetNextSel();
		}
	}
	return 0;
}
LRESULT CMainFrame::OnTabClose( WPARAM wParam,LPARAM lParam )
{
	PAGEID nPageID = (PAGEID)wParam;
	if(m_pUINotifyer)
	{
		m_pUINotifyer->NotifyPageClose(nPageID);
	}
	return 0;
}
LRESULT CMainFrame::OnDragTab( WPARAM wParam,LPARAM lParam )
{
	PAGEID nPageID = (PAGEID)wParam;
	if(m_pUINotifyer)
	{
		m_pUINotifyer->NotifyDragTab(nPageID);
	}
	return 0;
}
void CMainFrame::OnUpdateBackButton(CCmdUI *pUI)
{
	pUI->Enable(bCanBack);
}
void CMainFrame::OnUpdateForwardButton(CCmdUI *pUI)
{
	pUI->Enable(bCanForward);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	cs.hMenu = NULL;
	//cs.style|=WS_CLIPCHILDREN;
	cs.dwExStyle = WS_EX_WINDOWEDGE;
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	//CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	if(m_pUINotifyer)
	{
		CRect rcClient;
		this->GetClientRect(&rcClient);

		CRect rcReBar;
		m_wndRebar.GetWindowRect(&rcReBar);

		rcClient.top += rcReBar.Height();

		CRect rcStatusBar;
		m_wndStatusBar.GetWindowRect(rcStatusBar);

		rcClient.bottom-=rcStatusBar.Height();

		m_pUINotifyer->NotifySizeWindow(rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height());

	}

}
BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
 	if(bFirstErase && m_wndRebar.m_hWnd)
 	{
 		bFirstErase = FALSE;
 		CRect rcClient;
 		GetClientRect(rcClient);
 		CRect rcReBar;
 		m_wndRebar.GetWindowRect(rcReBar);
 
 		rcClient.top+=rcReBar.Height();
 		
 		pDC->FillSolidRect(rcClient,RGB(255,255,255));
 	}
	return true;
}
void CMainFrame::OnUICommand(UINT nCommandID)
{
	switch(nCommandID)
	{
	case ID_GO_HOME:
		{
			if(m_pUINotifyer)
				m_pUINotifyer->NotifyGoHome();
		}
		break;
	case ID_GO_BACK:
		{
			if(m_pUINotifyer)
				m_pUINotifyer->NotifyGoBack();
		}
		break;
	case ID_GO_FORWARD:
		{
			if(m_pUINotifyer)
				m_pUINotifyer->NotifyGoForward();
		}
		break;
	case ID_REFREASH:
		{
			if(m_pUINotifyer)
				m_pUINotifyer->NotifyRefresh();
		}
		break;
	case ID_STOP_LOAD:
		{
			if(m_pUINotifyer)
				m_pUINotifyer->NotifyStopLoading();
		}
		break;
	case ID_GOTO_URL:
		{
			if(m_pUINotifyer && m_pCurAddr)
			{
				CString strUrl;
				m_pCurAddr->GetWindowText(strUrl);
				m_pUINotifyer->NotifyGotoUrl(strUrl,false);
			}
		}
		break;
	case ID_NEW_TAB:
		{
			if(m_pUINotifyer && m_pCurAddr)
			{
				m_pUINotifyer->NotifyUINewWindow(NULL);
			}
		}
		break;
	case ID_SEARCH:
		{
			if (m_pUINotifyer)
			{
				m_pUINotifyer->NotifySearchText(m_wndNaviBar.GetSearchText());
			}
		}
		break;
	}
}

LRESULT CMainFrame::OnAsyncUrlChange(WPARAM wParam,LPARAM lParam)
{
	PAGEID nPageID = (PAGEID)wParam;
	LPCWSTR pszNewUrl = (LPCWSTR)lParam;

	if (g_UIMutiTabShow)
	{
		CIETabBar *pTabBar = m_wndTabBar.GetTabCtrl();
		if (pTabBar)
		{
			pTabBar->UpdateTabItemUrl((PVOID)nPageID,pszNewUrl);
		}
	}

	delete pszNewUrl;
	return 0L;
}

LRESULT CMainFrame::OnAsyncTitleChange(WPARAM wParam,LPARAM lParam)
{
	PAGEID nPageID = (PAGEID)wParam;
	LPCWSTR pszNewTitle = (LPCWSTR)lParam;

	CString strTitle;
	strTitle = pszNewTitle;
	strTitle = strTitle+TEXT(" - 网吧专用浏览器");

	if (g_UIMutiTabShow)
	{
		CIETabBar *pTabBar = m_wndTabBar.GetTabCtrl();
		if (pTabBar)
		{
			pTabBar->UpdateTabItemTitle((PVOID)nPageID,pszNewTitle);
		}
	}

	delete pszNewTitle;

	return 0;
}


LRESULT CMainFrame::OnAsyncStatusTextChange(WPARAM wParam,LPARAM lParam)
{
	LPCWSTR pszNewStatus = (LPCWSTR)wParam;
	if(m_wndStatusBar.m_hWnd && ::IsWindow(m_wndStatusBar.m_hWnd))
	{
		m_wndStatusBar.SetWindowText(pszNewStatus);
	}

	delete pszNewStatus;
	return 0;
}
LRESULT CMainFrame::OnAsyncFrameAddrChange(WPARAM wParam,LPARAM lParam)
{
	LPCWSTR pszNewAddr = (LPCWSTR)wParam;

	CIEComboBoxEx *pAddr = (CIEComboBoxEx *)m_wndNaviBar.GetAddrBarPtr();
	if (pAddr)
	{
		pAddr->AddOrChoiceUrlItem(pszNewAddr);
	}

	delete pszNewAddr;
	return 0;
}
LRESULT CMainFrame::OnAsyncFrameTitleChange(WPARAM wParam,LPARAM lParam)
{
	LPCWSTR pszNewTitle = (LPCWSTR)wParam;
	CString strTitle;
	strTitle = pszNewTitle;

	strTitle = strTitle+TEXT(" - 网吧专用浏览器");
	SetWindowText(strTitle);

	delete pszNewTitle;

	return 0;
}

typedef struct tagADD_TAB_PARAM
{
	LPCWSTR pszUrl;
	LPCWSTR pszTitle;
}ADD_TAB_PARAM,*PADD_TAB_PARAM;
LRESULT CMainFrame::OnAsyncAddTab(WPARAM wParam,LPARAM lParam)
{
	PAGEID nPageID = (PAGEID)wParam;
	PADD_TAB_PARAM pAddTabParam = (PADD_TAB_PARAM)lParam;
	if (nPageID == 0)
	{
		return 0;
	}

	if (g_UIMutiTabShow)
	{
		CIETabBar *pTabBar = m_wndTabBar.GetTabCtrl();
		if (pTabBar)
		{
			pTabBar->AddTabItem((PVOID)nPageID,pAddTabParam->pszUrl,pAddTabParam->pszTitle);
		}
	}
	

	delete pAddTabParam->pszUrl;
	delete pAddTabParam->pszTitle;
	delete pAddTabParam;

	return 0;
}

typedef struct tagDEL_TAB_PARAM
{	
	HANDLE hEvent;
	PAGEID *pFocusPageID;
}DEL_TAB_PARAM,*PDEL_TAB_PARAM;
LRESULT CMainFrame::OnAsyncDelTab(WPARAM wParam,LPARAM lParam)
{
	PAGEID nPageID = (PAGEID)wParam;
	PDEL_TAB_PARAM pDelParam = (PDEL_TAB_PARAM)lParam;
	if (nPageID == 0)
	{
		return 0;
	}
	PAGEID nNewPageID = 0;

	if (g_UIMutiTabShow)
	{
		CIETabBar *pTabBar = m_wndTabBar.GetTabCtrl();
		if (pTabBar)
		{
			nNewPageID = (PAGEID)pTabBar->RemoveTabItem((PVOID)nPageID);
		}
	}
	
	*(pDelParam->pFocusPageID) = nNewPageID;
	SetEvent(pDelParam->hEvent);

	delete pDelParam;

	return 0;
}
LRESULT CMainFrame::OnAsyncSetFocusTab(WPARAM wParam,LPARAM lParam)
{
	PAGEID nPageID = (PAGEID)wParam;
	if (nPageID == 0)
	{
		return 0;
	}

	if (g_UIMutiTabShow)
	{
		CIETabBar *pTabBar = m_wndTabBar.GetTabCtrl();
		if (pTabBar)
		{
			pTabBar->ChangeFocusTabItem((PVOID)nPageID);
		}
	}

	return 0;
}
bool CMainFrame::SetNofiyerPoint( IUINotifyer *pNotifyer )
{
	m_pUINotifyer = pNotifyer;
	return true;
}
bool CMainFrame::GetNofiyerPoint( IUINotifyer **ppNotifyer )
{
	if (ppNotifyer)
	{
		*ppNotifyer = m_pUINotifyer;
		return true;
	}
	return false;
}
unsigned long CMainFrame::ControlQueryQueryMainFrame(HWND *phMainFrame)
{
	if (phMainFrame)
	{
		*phMainFrame = this->m_hWnd;
	}
	return 0;
}
unsigned long CMainFrame::ControlQueryClientRect(LPRECT pRect)
{
	if(pRect)
	{
		CRect rcClient;
		this->GetClientRect(&rcClient);

		CRect rcReBar;
		m_wndRebar.GetWindowRect(&rcReBar);

		rcClient.top += rcReBar.Height();

		CRect rcStatusBar;
		m_wndStatusBar.GetWindowRect(rcStatusBar);

		rcClient.bottom-=rcStatusBar.Height();

		*pRect = rcClient;
	}
	return 0;
}

unsigned long CMainFrame::ControlPageUrlChange(PAGEID nPageID,const wchar_t * pszNewUrl) 
{
	if (pszNewUrl && wcslen(pszNewUrl) > 0 && nPageID != 0)
	{
		WCHAR *pszUrl = new WCHAR[wcslen(pszNewUrl)+1];
		wcscpy_s(pszUrl,wcslen(pszNewUrl)+1,pszNewUrl);
		PostMessage(WM_ASYNC_URL_CHANGE,nPageID,(LPARAM)pszUrl);
	}

	return 0;
};
unsigned long CMainFrame::ControlPageTitleChange(PAGEID nPageID,const wchar_t * pszNewTitle) 
{
	if (pszNewTitle && wcslen(pszNewTitle) > 0 && nPageID != 0)
	{
		WCHAR *pszTitle = new WCHAR[wcslen(pszNewTitle)+1];
		wcscpy_s(pszTitle,wcslen(pszNewTitle)+1,pszNewTitle);
		PostMessage(WM_ASYNC_TITLE_CHANGE,nPageID,(LPARAM)pszTitle);
	}
	return 0;
};
unsigned long CMainFrame::ControlFrameStatusChage(const wchar_t * pszNewStatus) 
{
	WCHAR *pszStatus = new WCHAR[wcslen(pszNewStatus)+1];
	wcscpy_s(pszStatus,wcslen(pszNewStatus)+1,pszNewStatus);
	PostMessage(WM_ASYNC_STATUS_TEXT_CHANGE,(LPARAM)pszStatus,0);
	return 0;
};
unsigned long CMainFrame::ControlFrameAddrChange(const wchar_t * pszNewAddr) 
{
	WCHAR *pszAddr = new WCHAR[wcslen(pszNewAddr)+1];
	wcscpy_s(pszAddr,wcslen(pszNewAddr)+1,pszNewAddr);
	PostMessage(WM_ASYNC_FRAME_ADDR_CHANGE,(LPARAM)pszAddr,0);
	return 0;
};

unsigned long CMainFrame::ControlFrameTitleChange(const wchar_t * pszNewTitle)
{

	WCHAR *pszFrameTitle = new WCHAR[wcslen(pszNewTitle)+1];
	wcscpy_s(pszFrameTitle,wcslen(pszNewTitle)+1,pszNewTitle);
	PostMessage(WM_ASYNC_FRAME_TITLE_CHANGE,(LPARAM)pszFrameTitle,0);

	return 0;
}
unsigned long CMainFrame::ControlBackForward(BOOL bCanBack,BOOL bCanForward)
{
	this->bCanBack = bCanBack;
	this->bCanForward = bCanForward;
	return 0;
};

unsigned long CMainFrame::ControlAddTab(PAGEID nPageID, LPCTSTR pszUrl,LPCTSTR pszTitle)
{
	WCHAR *pszAddUrl = new WCHAR[wcslen(pszUrl)+1];
	wcscpy_s(pszAddUrl,wcslen(pszUrl)+1,pszUrl);
	WCHAR *pszAddTitle = new WCHAR[wcslen(pszTitle)+1];
	wcscpy_s(pszAddTitle,wcslen(pszTitle)+1,pszTitle);
	PADD_TAB_PARAM pAddTabParam = new ADD_TAB_PARAM;
	pAddTabParam->pszUrl = pszAddUrl;
	pAddTabParam->pszTitle = pszAddTitle;

	PostMessage(WM_ASYNC_ADD_TAB,nPageID,(LPARAM)pAddTabParam);
	return 0; 
};

unsigned long CMainFrame::ControlDelTab( PAGEID nPageID )
{
// 	HANDLE hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
// 	PAGEID nNewPageID = 0;
// 	PDEL_TAB_PARAM pDelParam = new DEL_TAB_PARAM;
// 	pDelParam->hEvent = hEvent;
// 	pDelParam->pFocusPageID = &nNewPageID;
// 	PostMessage(WM_ASYNC_DEL_TAB,(WPARAM)nPageID,0);
// 	WaitForSingleObject(hEvent,INFINITE);
// 	CloseHandle(hEvent);
// 	return nNewPageID;


// 	PAGEID nPageID = (PAGEID)wParam;
// 	PDEL_TAB_PARAM pDelParam = (PDEL_TAB_PARAM)lParam;
	if (nPageID == 0)
	{
		return 0;
	}
	PAGEID nNewPageID = 0;

	if (g_UIMutiTabShow)
	{
		CIETabBar *pTabBar = m_wndTabBar.GetTabCtrl();
		if (pTabBar)
		{
			nNewPageID = (PAGEID)pTabBar->RemoveTabItem((PVOID)nPageID);
		}
	}
	
// 	*(pDelParam->pFocusPageID) = nNewPageID;
// 	SetEvent(pDelParam->hEvent);
// 
// 	delete pDelParam;

	return nNewPageID;
}

unsigned long CMainFrame::ControlSetFocusTab( PAGEID nPageID )
{
	PostMessage(WM_ASYNC_SET_FOCUS_TAB,(WPARAM)nPageID,0);
	return 0;
}
void CMainFrame::OnClose()
{
    AfxGetApp()->WriteProfileString(_T("settting"),_T("pos"),_T(""));
    WINDOWPLACEMENT place = { 0 };
    place.length = sizeof(WINDOWPLACEMENT);
    if (GetWindowPlacement(&place))
    {
        place.flags = 0;
        if (IsIconic())
        {
            place.flags |= WPF_SETMINPOSITION;
            place.showCmd |= SW_SHOWMINIMIZED;
        }
        CString szSize;
        szSize.Format(_T("%u,%u,%d,%d,%d,%d,%d,%d,%d,%d"),
            place.flags,
            place.showCmd,
            place.ptMinPosition.x,
            place.ptMinPosition.y,
            place.ptMaxPosition.x,
            place.ptMaxPosition.y,
            place.rcNormalPosition.left,
            place.rcNormalPosition.right,
            place.rcNormalPosition.top,
            place.rcNormalPosition.bottom);
        AfxGetApp()->WriteProfileString(_T("settting"),_T("pos"),szSize);
    }
	BOOL bCanClose = TRUE;
	if (m_pUINotifyer)
	{
		m_pUINotifyer->NotifyFrameClose(&bCanClose);
	}
	if(bCanClose)
	{
		CFrameWnd::OnClose();
		PostQuitMessage(0);
	}
}



BOOL CMainFrame::OnNcActivate(BOOL bActive)
{
	return __super::OnNcActivate(bActive);
}

void CMainFrame::OnPrintWebView()
{
	if( m_pUINotifyer )
	{
		m_pUINotifyer->NotifyPrintWebView(0);
	}
}
