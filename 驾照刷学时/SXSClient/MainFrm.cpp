
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "SXSClient.h"
#include "MainFrm.h"
#include "浏览器自动化/VirtualMouse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_NCDESTROY()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER+2222,OnShellIcon)
	ON_COMMAND(ID_ROOT_SHOW, &CMainFrame::OnRootShow)
	ON_COMMAND(ID_ROOT_HIDE, &CMainFrame::OnRootHide)
	ON_COMMAND(ID_ROOT_EXIT, &CMainFrame::OnRootExit)
	ON_COMMAND(ID_APP_HELP, &CMainFrame::OnAppHelp)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	m_pView = NULL;
}

CMainFrame::~CMainFrame()
{
	if (m_pView)
	{
		delete m_pView;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetIcon(LoadIcon(theApp.m_hInstance,MAKEINTRESOURCEW(IDR_MAINFRAME)),TRUE);
	SetIcon(LoadIcon(theApp.m_hInstance,MAKEINTRESOURCEW(IDR_MAINFRAME)),FALSE);
// 	CMenu *pMainmenu = new CMenu;
// 	pMainmenu->LoadMenu(IDR_MAINFRAME);
// 	SetMenu(pMainmenu);

	UpdateFrameTitle(theApp.m_strUserName);



	m_pView = new CSXSView;

	CRect rcClient;
	GetClientRect(&rcClient);
	m_pView->Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcClient,this,0);

	return 0;
}
IDispatch * CMainFrame::CreateInstance(LPCTSTR pszUrl)
{
	if(Create(NULL,NULL))
	{
		ShowWindow(SW_SHOW);
		if(pszUrl && wcslen(pszUrl) > 0)
		{
			m_pView->Navigate2(pszUrl);
		}
		return m_pView->GetApplication();
	}
	return NULL;
}

HWND CMainFrame::GetIEServerWnd()
{
	if (m_pView)
	{
		return m_pView->GetIEServerWnd();
	}
	return NULL;
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	cs.hMenu = NULL;
	cs.dwExStyle = WS_EX_WINDOWEDGE;

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
	//return CFrameWnd::OnEraseBkgnd(pDC);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	if (m_pView)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		m_pView->MoveWindow(&rcClient,FALSE);
	}

}

void CMainFrame::OnNcDestroy()
{
	CFrameWnd::OnNcDestroy();

	delete this;
}

void CMainFrame::OnClose()
{
	//ShowWindow(SW_HIDE);

	//CFrameWnd::OnClose();
}

LRESULT CMainFrame::OnShellIcon(WPARAM wParam,LPARAM lParam)
{
	if ( wParam == 1234 )
	{
		if (lParam == WM_LBUTTONDOWN)
		{
			if (IsWindowVisible())
			{
				ShowWindow(SW_HIDE);
			}
			else
			{
				ShowWindow(SW_SHOW);
				SetForegroundWindow();
			}
			
		}

		if (lParam == WM_RBUTTONDOWN)
		{
			CPoint ptCursor;
			GetRealMousePos(&ptCursor);

			CMenu PopMenu;
			PopMenu.LoadMenu(IDR_NOTIFY_POPUP);
			PopMenu.GetSubMenu(0)->TrackPopupMenu(TPM_RIGHTALIGN|TPM_BOTTOMALIGN,ptCursor.x,ptCursor.y,this);
		}
	}
	return 0;
}


void CMainFrame::UpdateFrameTitle(LPCWSTR pszTitle)
{
	CString strTitle;
	strTitle = pszTitle;
	SetWindowTextW(strTitle+L" - 驾照刷学时 V1.1");

	BOOL bHasAdd = NotifyData.cbSize == sizeof(NOTIFYICONDATAW);
	if ( FALSE == bHasAdd )
	{
		NotifyData.cbSize = sizeof(NOTIFYICONDATAW);
		NotifyData.uID = 1234;
		NotifyData.hIcon = (HICON)LoadIcon(theApp.m_hInstance,MAKEINTRESOURCEW(IDR_MAINFRAME));
		NotifyData.hWnd = m_hWnd;
		NotifyData.uCallbackMessage = WM_USER+2222;
		wcscpy_s(NotifyData.szTip,128,pszTitle);
		NotifyData.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE ;

		BOOL bRes = Shell_NotifyIconW(NIM_ADD, &NotifyData);
	}
	else
	{
		wcscpy_s(NotifyData.szTip,128,pszTitle);
		BOOL bRes = Shell_NotifyIconW(NIM_MODIFY, &NotifyData);
	}



}

void CMainFrame::OnRootShow()
{
	ShowWindow(SW_SHOW);
	SetForegroundWindow();
}

void CMainFrame::OnRootHide()
{
	ShowWindow(SW_HIDE);
}

void CMainFrame::OnRootExit()
{
	Shell_NotifyIconW(NIM_DELETE, &NotifyData);
	CFrameWnd::OnClose();
}

void CMainFrame::OnAppHelp()
{
	AfxMessageBox(L"启动参数\r\n -username 用户名 -password 密码");
}
