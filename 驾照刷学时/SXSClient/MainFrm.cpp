
// MainFrm.cpp : CMainFrame ���ʵ��
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
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CMainFrame ����/����

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

	UpdateFrameTitle(theApp.m_strUserName);

	NotifyData.cbSize = sizeof(NOTIFYICONDATAW);
	NotifyData.hIcon = (HICON)LoadIcon(theApp.m_hInstance,MAKEINTRESOURCEW(IDR_MAINFRAME));
	NotifyData.hWnd = m_hWnd;
	GetWindowText(NotifyData.szTip,128);
	NotifyData.uFlags = NIF_ICON|NIF_TIP ;

	BOOL bRes = Shell_NotifyIconW(NIM_ADD, &NotifyData);

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

// CMainFrame ���

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


// CMainFrame ��Ϣ�������

BOOL CMainFrame::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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
	ShowWindow(SW_HIDE);

	//CFrameWnd::OnClose();
}

void CMainFrame::UpdateFrameTitle(LPCWSTR pszTitle)
{
	CString strTitle;
	strTitle = pszTitle;
	SetWindowTextW(strTitle+L" - ����ˢѧʱ V1.0");
}
