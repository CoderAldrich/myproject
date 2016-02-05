#include "stdafx.h"

#include "IE8NaviBGWnd.h"
BEGIN_MESSAGE_MAP(CIE8NaviBarBGWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_STATUS_CHANGE,OnStatusChage)
	ON_WM_SIZE()
END_MESSAGE_MAP()


CIE8NaviBarBGWnd::CIE8NaviBarBGWnd()
{
	m_bmpNaviBG.LoadBitmapW(IDB_NAVI_BG);
	m_hMemDC = CreateCompatibleDC(NULL);
	::SelectObject(m_hMemDC,m_bmpNaviBG);
	m_nStatus = 0;
}
CIE8NaviBarBGWnd::~CIE8NaviBarBGWnd()
{

}

void CIE8NaviBarBGWnd::OnPaint()
{
	CPaintDC dc(this); 
}

int CIE8NaviBarBGWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndNaviBar.CreateEx(this , TBSTYLE_FLAT|TBSTYLE_TRANSPARENT);

	m_wndNaviBar.m_wndDropDown.RegisterStatusChangeNotify(m_hWnd,WM_STATUS_CHANGE);

	RECT rc={0,0,400,400};
	m_wndAddr.Create(WS_VISIBLE|WS_CHILD|CBS_DROPDOWN|CBS_AUTOHSCROLL|WS_CLIPCHILDREN,rc,this,0);
	m_wndNaviCmdBar.CreateEx(this, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT);
	m_wndSearch.Create(NULL,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN,CRect(0,0,200,20),this,0);
	
	return 0;
}


BOOL CIE8NaviBarBGWnd::OnEraseBkgnd(CDC* pDC)
{
	POINT pt = { 0, 0 };
	MapWindowPoints(GetParent(), &pt, 1);
	CPoint orgPt = pDC->OffsetWindowOrg(pt.x, pt.y);

	HRESULT hr =   GetParent()->SendMessage(WM_ERASEBKGND,(WPARAM)(pDC->m_hDC),0);

	pDC->SetWindowOrg(orgPt.x, orgPt.y);


	BITMAP tmpBmp;
	m_bmpNaviBG.GetBitmap(&tmpBmp);

	CRect rcWin;
	GetClientRect(&rcWin);

	::TransparentBlt(pDC->m_hDC,rcWin.left+1,rcWin.top+1,74/*rcWin.Width()*/,29,m_hMemDC,m_nStatus*74,0,74,29,RGB(212,208,200));

	return true;
}

LRESULT CIE8NaviBarBGWnd::OnStatusChage(WPARAM wParam,LPARAM lParam)
{
	m_nStatus = wParam;
	CRect rcWin;
	this->GetClientRect(&rcWin);
	this->InvalidateRect(rcWin);
	return 0;
}

void CIE8NaviBarBGWnd::RelayoutChildWindow(void)
{

	static const UINT nSearhWidth = 300;

	CRect rcWin;
	GetClientRect(&rcWin);
	CRect oldrcWin;
	oldrcWin = rcWin;

	CRect rcTempItem;
	m_wndNaviCmdBar.GetItemRect(m_wndNaviCmdBar.GetToolBarCtrl().GetButtonCount()-1,&rcTempItem);

	if ( (int)( oldrcWin.Width() - ( rcTempItem.right+nSearhWidth)) > 300)
	{
		rcWin.left=rcWin.right-nSearhWidth;
		m_wndSearch.MoveWindow(&rcWin);

		rcWin.left=0;
		rcWin.right=74;
		m_wndNaviBar.MoveWindow(&rcWin);


		CRect rcItem;
		m_wndNaviCmdBar.GetItemRect(m_wndNaviCmdBar.GetToolBarCtrl().GetButtonCount()-1,&rcItem);

		rcWin.left=oldrcWin.right-nSearhWidth-(rcItem.right+3);
		rcWin.right=oldrcWin.right-nSearhWidth;
		rcWin.top+=2;
		rcWin.bottom+=2;
		m_wndNaviCmdBar.MoveWindow(&rcWin);

		CRect rcAddr;
		m_wndAddr.GetWindowRect(&rcAddr);

		rcWin.left=80;
		rcWin.right=oldrcWin.right-nSearhWidth-(rcItem.right+3);
		rcWin.top = (oldrcWin.Height()-rcAddr.Height())/2+1;
		rcWin.top -= 2;
		rcWin.bottom -= 2;
		m_wndAddr.MoveWindow(&rcWin);
	}


}

void CIE8NaviBarBGWnd::OnSize(UINT nType, int cx, int cy)
{
	//CControlBar::OnSize(nType, cx, cy);

	RelayoutChildWindow();

}
CComboBox * CIE8NaviBarBGWnd::GetAddrBarPtr(void)
{
	return &m_wndAddr;
}

CString CIE8NaviBarBGWnd::GetSearchText()
{
	return m_wndSearch.GetSearchText();
}
CSize CIE8NaviBarBGWnd::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	return CSize(30,30);
}
CSize CIE8NaviBarBGWnd::CalcDynamicLayout(int nLength, DWORD nMode)
{
	return CSize(30,30);
}