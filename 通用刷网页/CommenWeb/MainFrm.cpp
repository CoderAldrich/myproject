
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
//#include "ShuaClient.h"
#include "MainFrm.h"

//#include "VirtualMouse.h"
//#include "RandHome.h"
//#include "SAStatusLog.h"

#include "PublicFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_NCDESTROY()
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


extern UINT    g_nMatrixShowNum;
extern UINT    g_nMatrixClickRate;

CMainFrame::CMainFrame()
{
	m_pView = NULL;
}

CMainFrame::~CMainFrame()
{
	if (m_pView)
	{
		//delete m_pView;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pView = new CIECoreView();

	CRect rcClient;
	GetClientRect(&rcClient);
	m_pView->Create(NULL,NULL,WS_VISIBLE|WS_CHILD,rcClient,this,0);

	return 0;
}
IDispatch * CMainFrame::CreateInstance()
{
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rt,0) ;   // 获得工作区大小

	BOOL bRes = Create(NULL,NULL,WS_OVERLAPPEDWINDOW,rt);
	if(bRes)
	{
		ShowWindow(SW_SHOW);

		return m_pView->GetApplication();
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
	return TRUE;
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

	//delete this;
}


void CMainFrame::OnClose()
{
	CFrameWnd::OnClose();
}

void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (bCalcValidRects)
	{
		int xFrame = -10; //左右边框的厚度
		int yFrame = -10; //下边框的厚度
		int nTHight = -10; //标题栏的高度
		RECT    * rc;
		rc = (RECT *)&(lpncsp->rgrc);
		rc->left   = rc->left + xFrame;
		rc->top    = rc->top + nTHight;
		rc->right  = rc->right - xFrame;
		rc->bottom = rc->bottom - yFrame;
	}
	//CFrameWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}
