// IE8TabBarBGWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "UIPublic.h"
#include "IE8TabBarBGWnd.h"
#include "BrowserPublic.h"

// CIE8TabBarBGWnd
extern BOOL              g_UICommandBarShow;
IMPLEMENT_DYNAMIC(CIE8TabBarBGWnd, CControlBar)

CIE8TabBarBGWnd::CIE8TabBarBGWnd():m_wndTabBar(4,180,10)
{
	m_Pen.CreatePen(PS_SOLID,1,RGB(163,167,172));
	m_bBaseTheme = GetIsWindowBaseTheme();
}

CIE8TabBarBGWnd::~CIE8TabBarBGWnd()
{
}


BEGIN_MESSAGE_MAP(CIE8TabBarBGWnd, CControlBar)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CIE8TabBarBGWnd 消息处理程序

void CIE8TabBarBGWnd::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
}


int CIE8TabBarBGWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndFavBtn.CreateEx(this,TBSTYLE_TRANSPARENT|TBSTYLE_LIST);//,WS_VISIBLE|WS_CHILD|CBRS_ALIGN_TOP,CRect(0,0,50,30));
	m_wndCmdBar.CreateEx(this,TBSTYLE_TRANSPARENT|TBSTYLE_LIST|TBSTYLE_FLAT|TBSTYLE_AUTOSIZE);


	m_wndFavBtn.AddButton(0,ID_NOTHING_ENABLE,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,0);
	m_wndFavBtn.SetButtonText(0,TEXT("收藏夹"));
	m_wndFavBtn.SetButtonBorder(true);
	

	CRect rcFirst;
	m_wndFavBtn.GetItemRect(0,&rcFirst);
	m_wndFavBtn.MoveWindow(0,(30-rcFirst.Height())/2,rcFirst.Width(),rcFirst.Height());
	m_wndFavBtn.SetImageList(IDB_IE8_CMD,16,16);



	for (int i=0;i<8;i++)
	{
		m_wndCmdBar.AddButton(i,ID_NOTHING_ENABLE,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,i);
	}

	m_wndCmdBar.SetButtonBorder(true);
	m_wndCmdBar.SetImageList(IDB_IE8_CMD,16,16);
	m_wndCmdBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	for (int i=0;i< m_wndCmdBar.GetToolBarCtrl().GetButtonCount() ;i++)
	{
		m_wndCmdBar.SetButtonStyle(i,m_wndCmdBar.GetButtonStyle(i)|TBBS_AUTOSIZE|(i!=2?BTNS_DROPDOWN:0));
	}

	if(m_bBaseTheme == false)
	{
		m_wndCmdBar.SetButtonSpace(0);
		m_wndCmdBar.SetButtonPad(4,0);
	}


	m_wndCmdBar.SetButtonText(4,TEXT("页面(&P)"));
	m_wndCmdBar.SetButtonText(5,TEXT("安全(&S)"));
	m_wndCmdBar.SetButtonText(6,TEXT("工具(&O)"));


	m_wndCmdBar.SetButtonInfo(0,ID_GO_HOME,m_wndCmdBar.GetButtonStyle(0),4);
	m_wndCmdBar.SetButtonInfo(1,ID_NOTHING_ENABLE,m_wndCmdBar.GetButtonStyle(1),1);
	m_wndCmdBar.SetButtonInfo(2,ID_NOTHING_ENABLE,m_wndCmdBar.GetButtonStyle(2),6);
	m_wndCmdBar.SetButtonInfo(3,ID_NOTHING_ENABLE,m_wndCmdBar.GetButtonStyle(3),2);
	m_wndCmdBar.SetButtonInfo(4,ID_NOTHING_ENABLE,m_wndCmdBar.GetButtonStyle(4)|BTNS_WHOLEDROPDOWN, I_IMAGENONE );
	m_wndCmdBar.SetButtonInfo(5,ID_NOTHING_ENABLE,m_wndCmdBar.GetButtonStyle(5)|BTNS_WHOLEDROPDOWN, I_IMAGENONE );
	m_wndCmdBar.SetButtonInfo(6,ID_NOTHING_ENABLE,m_wndCmdBar.GetButtonStyle(6)|BTNS_WHOLEDROPDOWN, I_IMAGENONE );
	m_wndCmdBar.SetButtonInfo(7,ID_NOTHING_ENABLE,m_wndCmdBar.GetButtonStyle(7)|BTNS_WHOLEDROPDOWN,3);


	m_wndTabBar.Create(WS_VISIBLE|WS_CHILD|TCS_FIXEDWIDTH/*|WS_CLIPCHILDREN*/|TCS_TOOLTIPS/* |TCS_OWNERDRAWFIXED*/ ,CRect(80/*rcFirst.Width()*/,0,1000,30),this,123);
	
	if (FALSE == g_UICommandBarShow)
	{
		m_wndCmdBar.ShowWindow(SW_HIDE);
	}
	return 0;
}
CSize CIE8TabBarBGWnd::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	return CSize(30,30);
}
CSize CIE8TabBarBGWnd::CalcDynamicLayout(int nLength, DWORD nMode)
{
	return CSize(30,30);
}
BOOL CIE8TabBarBGWnd::OnEraseBkgnd(CDC* pDC)
{

	POINT pt = { 0, 0 };
	MapWindowPoints(GetParent(), &pt, 1);
	CPoint orgPt = pDC->OffsetWindowOrg(pt.x, pt.y);

	HRESULT hr =   GetParent()->SendMessage(WM_ERASEBKGND,(WPARAM)(pDC->m_hDC),0);

	pDC->SetWindowOrg(orgPt.x, orgPt.y);
	
	CRect rcWin;
	GetWindowRect(&rcWin);

	//pDC->FillSolidRect(0,rcWin.Height()-5,rcWin.Width(),5,RGB(255,0,0));

	if(m_bBaseTheme == FALSE)
	{
		UINT nBottomHeight = 4;
		UINT nTopOffset = rcWin.Height()-nBottomHeight;
		pDC->FillSolidRect(0,nTopOffset,rcWin.Width(),nBottomHeight,RGB(227,238,251));
		pDC->SelectObject(m_Pen);
		pDC->MoveTo(0,nTopOffset);
		pDC->LineTo(rcWin.Width(),nTopOffset);
	}
	

	return true;

	return CControlBar::OnEraseBkgnd(pDC);
}

void CIE8TabBarBGWnd::OnSize(UINT nType, int cx, int cy)
{
	CControlBar::OnSize(nType, cx, cy);

	CRect rcTab;
	m_wndTabBar.GetWindowRect(&rcTab);
	ScreenToClient(&rcTab);
	if (TRUE == g_UICommandBarShow)
	{
		rcTab.right = cx-380;
		if(rcTab.Width() > 300)
		{
			m_wndTabBar.MoveWindow(rcTab);

			CRect rcItem;
			m_wndCmdBar.GetToolBarCtrl().GetItemRect(0,&rcItem);

			m_wndCmdBar.MoveWindow(rcTab.right,rcTab.top+(rcTab.Height()-rcItem.Height())/2,380,rcItem.Height());
		}
	}
	else
	{
		rcTab.right = cx;
		m_wndTabBar.MoveWindow(rcTab);
	}
}

CIETabBar * CIE8TabBarBGWnd::GetTabCtrl()
{
	return &m_wndTabBar;
}