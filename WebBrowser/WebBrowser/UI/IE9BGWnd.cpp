// IE9BGWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "UIPublic.h"
#include "IE9BGWnd.h"


// CIE9BGWnd

IMPLEMENT_DYNAMIC(CIE9BGWnd, CControlBar)

CIE9BGWnd::CIE9BGWnd():m_wndTabBar(0,180,10)
{
	m_bHideTabBar = FALSE;
}

CIE9BGWnd::~CIE9BGWnd()
{
}


BEGIN_MESSAGE_MAP(CIE9BGWnd, CControlBar)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CIE9BGWnd 消息处理程序

void CIE9BGWnd::OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL /*bDisableIfNoHndler*/)
{
}
CSize CIE9BGWnd::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	return CSize(35,35);
}
CSize CIE9BGWnd::CalcDynamicLayout(int nLength, DWORD nMode)
{
	return CSize(35,35);
}

BOOL CIE9BGWnd::OnEraseBkgnd(CDC* pDC)
{
	POINT pt = { 0, 0 };
	MapWindowPoints(GetParent(), &pt, 1);
	CPoint orgPt = pDC->OffsetWindowOrg(pt.x, pt.y);
	HRESULT hr =   GetParent()->SendMessage(WM_ERASEBKGND,(WPARAM)(pDC->m_hDC),0);
	pDC->SetWindowOrg(orgPt.x, orgPt.y);

	return true;
}



int CIE9BGWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndTabBar.Create(WS_VISIBLE|WS_CHILD|TCS_FIXEDWIDTH,CRect(80/*rcFirst.Width()*/,0,1000,26),this,123);
	m_wndCmdBar.CreateEx(this,TBSTYLE_TRANSPARENT|TBSTYLE_FLAT);
	m_wndNaviBar.CreateEx(this,TBSTYLE_TRANSPARENT|TBSTYLE_FLAT);


	m_wndNaviBar.AddButton(0,ID_NOTHING_ENABLE,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,0);
	m_wndNaviBar.AddButton(1,ID_NOTHING_ENABLE,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,1);
	m_wndNaviBar.SetButtonSpace(3);
	m_wndNaviBar.SetButtonPad(0,0);



	m_wndCmdBar.AddButton(0,ID_NOTHING_ENABLE,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,0);
	m_wndCmdBar.AddButton(1,ID_NOTHING_ENABLE,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,1);
	m_wndCmdBar.AddButton(2,ID_NOTHING_ENABLE,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,2);


	const UINT nNaviWidht = 38;
	const UINT nNaviHeight = 35;
	m_wndNaviBar.SetImageList( IDB_IE9_NAVI_NORMAL,nNaviWidht,nNaviWidht);
	m_wndNaviBar.SetHotImageList( IDB_IE9_NAVI_HOT,nNaviWidht,nNaviWidht);

	const UINT nCmdWidht = 17;
	const UINT nCmdHeight = 16;
	m_wndCmdBar.SetImageList(IDB_IE9_CMD_NORMAL,nCmdWidht,nCmdHeight);
	m_wndCmdBar.SetHotImageList(IDB_IE9_CMD_HOT,nCmdWidht,nCmdHeight);
	m_wndCmdBar.SetPressImageList(IDB_IE9_CMD_PRESS,nCmdWidht,nCmdHeight);

	//m_wndCmdBar.SetButtonBorder(true);

	m_wndAddr.Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN,CRect(0,0,30,30),this,0);

	return 0;
}

void CIE9BGWnd::OnSize(UINT nType, int cx, int cy)
{

	const UINT nTabBarHeight = 28;
	const UINT nCmdBarWidth = 80;
	const UINT nNaviBarWidht = 80;
	const UINT nTopOffset = 5;

	BOOL bRepaint = TRUE;// ( SIZE_MAXIMIZED == nType );
	m_wndNaviBar.MoveWindow(0,0,nNaviBarWidht,cy,bRepaint);
	m_wndCmdBar.MoveWindow(cx-nCmdBarWidth,nTopOffset,nCmdBarWidth,cy-nTopOffset,bRepaint);

	if( FALSE == m_bHideTabBar )
	{
		m_wndAddr.MoveWindow(nNaviBarWidht,nTopOffset+1,(cx-nCmdBarWidth-nNaviBarWidht)/2,28,bRepaint);
		m_wndTabBar.MoveWindow(nNaviBarWidht+(cx-nCmdBarWidth-nNaviBarWidht)/2,cy-nTabBarHeight,(cx-nCmdBarWidth-nNaviBarWidht)/2,nTabBarHeight,bRepaint);
	}
	else
	{
		m_wndAddr.MoveWindow(nNaviBarWidht,nTopOffset+1,(cx-nCmdBarWidth-nNaviBarWidht),28,bRepaint);
	}


}

CComboBox * CIE9BGWnd::GetAddrBarPtr()
{
	return m_wndAddr.GetAddrPtr();
}
CIETabBar * CIE9BGWnd::GetTabBarPtr()
{
	return &m_wndTabBar;
}

VOID CIE9BGWnd::HideTabBar()
{
	m_wndTabBar.ShowWindow(SW_HIDE);
	m_bHideTabBar = TRUE;
}