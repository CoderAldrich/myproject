
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "SubWindowRunner.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_USER+2222,OnNewWindowAttach)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	
}

CMainFrame::~CMainFrame()
{

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(1000,5000,NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.dwExStyle|=WS_EX_TOOLWINDOW;
	cs.lpszClass = AfxRegisterWndClass(0);
// 	cs.x = -1000;
// 	cs.y = -1000;
// 	cs.cx = 1000;
// 	cs.cy = 1000;

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

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{

}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 否则，执行默认处理
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	pDC->FillSolidRect(&rcClient,RGB(255,255,255));
	return TRUE;

	//return CFrameWnd::OnEraseBkgnd(pDC);
}

LRESULT CMainFrame::OnNewWindowAttach(WPARAM wParam,LPARAM lParam)
{
	HWND hNewWnd = (HWND)wParam;

	CRect rcClient;
	GetClientRect(&rcClient);
	::MoveWindow( hNewWnd,rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height(),TRUE);

	return 0;
}

BOOL CALLBACK MYWNDENUMPROC(HWND hWnd, LPARAM lParam)
{
	HWND *pWnd = (HWND *)lParam;
	if (pWnd)
	{
		*pWnd = hWnd;
	}
	return FALSE;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if ( 1000 == nIDEvent )
	{
		KillTimer(nIDEvent);
		HWND hChildWnd = NULL;
		::EnumChildWindows(m_hWnd,MYWNDENUMPROC,(LPARAM)&hChildWnd);
		
		CRect rcClient;
		GetClientRect(&rcClient);
		::MoveWindow( hChildWnd,rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height(),TRUE);

	}

	CFrameWnd::OnTimer(nIDEvent);
}
