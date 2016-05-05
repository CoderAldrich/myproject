
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "SXSClient.h"
#include "MainFrm.h"

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
