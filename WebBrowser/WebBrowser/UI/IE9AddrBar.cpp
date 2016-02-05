#include "stdafx.h"

#include "IE9AddrBar.h"
BEGIN_MESSAGE_MAP(CIE9AddrBar, CControlBar)
	//ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	//ON_COMMAND(ID_ADDR_FOCUS,OnAddrFocus)
END_MESSAGE_MAP()


CIE9AddrBar::CIE9AddrBar()
{
	//m_defaultfont.Attach((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	VERIFY(m_defaultfont.CreateFont(
		16,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Calibri")));                 // lpszFacename


	m_defaulticon = LoadIconW(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ADDR_BAR_DEFAULT));

}
CIE9AddrBar::~CIE9AddrBar()
{

}

void CIE9AddrBar::OnPaint()
{
	CPaintDC dc(this); 
}

int CIE9AddrBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;


	m_wndTravBar.CreateEx(this,TBSTYLE_TRANSPARENT|TBSTYLE_FLAT);

	m_wndTravBar.AddButton(0,ID_NOTHING_ENABLE,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,0);
	m_wndTravBar.AddButton(1,ID_NOTHING_ENABLE,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,1);

	const UINT nTravWidht = 16;
	const UINT nTravHeight = 16;

	m_wndTravBar.SetImageList(IDB_IE9_NAVI_CMD_NORMAL,nTravWidht,nTravHeight);
	m_wndTravBar.SetHotImageList(IDB_IE9_NAVI_CMD_HOT,nTravWidht,nTravHeight);
	m_wndTravBar.SetPressImageList(IDB_IE9_NAVI_CMD_PRESS,nTravWidht,nTravHeight);
	m_wndTravBar.SetButtonSpace(3);
	m_wndTravBar.SetButtonPad(0,0);

	RECT rc={0,0,0,20};
	m_wndAddr.Create(WS_VISIBLE|WS_CHILD|CBS_DROPDOWN|WS_CLIPCHILDREN/*|CBS_OWNERDRAWVARIABLE*/,CRect(0,0,400,24),this,0);
	m_wndAddr.SetFont(&m_defaultfont);

	HWND hEdit = ::FindWindowEx(m_wndAddr.m_hWnd,NULL,TEXT("Edit"),NULL);

	m_wndEdit.SubclassWindow(hEdit);

	return 0;
}


BOOL CIE9AddrBar::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;

	GetWindowRect(&rcClient);
	ScreenToClient(&rcClient);

	pDC->FillSolidRect(&rcClient,RGB(255,255,255));
	pDC->DrawEdge(rcClient,EDGE_ETCHED,BF_RECT);

 	CRect rcWin;
 	GetWindowRect(&rcWin);
 	ScreenToClient(&rcWin);
 
 	::DrawIconEx(pDC->m_hDC,(rcWin.Height()-16)/2,(rcWin.Height()-16)/2,m_defaulticon,16,16,0,NULL,DI_NORMAL);

	return true;
}


afx_msg void CIE9AddrBar::OnAddrFocus()
{
	//m_wndAddrEdit.ShowWindow(SW_SHOW);
}
void CIE9AddrBar::RelayoutChildWindow(void)
{

}

void CIE9AddrBar::OnSize(UINT nType, int cx, int cy)
{
	RelayoutChildWindow();

	//

 	CRect rcFirstItem;
 	m_wndTravBar.GetItemRect(0,&rcFirstItem);
 
 	TBMETRICS tm;
 	tm.cbSize = sizeof(tm);
 	tm.dwMask = TBMF_BUTTONSPACING;
 
 	m_wndTravBar.GetToolBarCtrl().GetMetrics(&tm);
 
 	const UINT TravWidht = rcFirstItem.Width()*2+tm.cxButtonSpacing;
 	const UINT LeftOffset = cy-6;
 	m_wndTravBar.MoveWindow(cx-TravWidht-5,(cy-rcFirstItem.Height())/2,TravWidht,rcFirstItem.Height());
 
 	m_wndAddr.MoveWindow(1+LeftOffset,2,cx-TravWidht-8-LeftOffset,24/*cy-2*//*rcFirstItem.Height()*/);

	CRect rcWin;
	m_wndAddr.GetWindowRect(&rcWin);
	m_wndAddr.ScreenToClient(&rcWin);
	m_wndAddr.InvalidateRect(&rcWin);

	GetWindowRect(&rcWin);
	ScreenToClient(&rcWin);
	InvalidateRect(&rcWin);

}

CComboBox * CIE9AddrBar::GetAddrPtr()
{
	return &m_wndAddr;
}
BEGIN_MESSAGE_MAP(CMyComboBox, CComboBox)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_ERASEBKGND()
ON_WM_KEYDOWN()
ON_CONTROL_REFLECT(CBN_SELCHANGE, &CMyComboBox::OnCbnSelchange)
ON_CONTROL_REFLECT(CBN_SELENDOK, &CMyComboBox::OnCbnSelendok)
END_MESSAGE_MAP()


void CMyComboBox::Draw(CDC *pDC)
{
	CDC dc;
	dc.Attach(pDC->m_hDC);

	CRect rcWin;
	GetWindowRect(&rcWin);
	ScreenToClient(&rcWin);

	//rcWin.DeflateRect(2,2,2,2);

	rcWin.bottom = rcWin.top+20;
	dc.FillSolidRect(rcWin,RGB(255,255,255));


	COMBOBOXINFO ci;
	ci.cbSize = sizeof(ci);
	GetComboBoxInfo(&ci);

	const UINT nDropWidth = 6;
	const UINT nDropHeight = 3;

	CRect rcTemp;
	rcTemp = ci.rcButton;

	CRect rcDrop;
	rcDrop.left = rcTemp.left+(rcTemp.Width()-nDropWidth)/2;
	rcDrop.right = rcDrop.left+nDropWidth;
	rcDrop.top  = rcTemp.top+(rcTemp.Height()-nDropHeight)/2;
	rcDrop.bottom = rcDrop.top+nDropHeight;

	dc.SelectObject(&m_dropBrush);
	dc.BeginPath();  
	dc.MoveTo(rcDrop.left,rcDrop.top);  
	dc.LineTo(rcDrop.right,rcDrop.top);  
	dc.LineTo(rcDrop.left+rcDrop.Width()/2,rcDrop.bottom);  
	dc.LineTo(rcDrop.left,rcDrop.top);  
	dc.EndPath();  
	dc.FillPath(); 

	//pDC->SelectObject(&m_dropBrush);
	//pDC->BeginPath();  
	//pDC->MoveTo(rcDrop.left,rcDrop.top);  
	//pDC->LineTo(rcDrop.right,rcDrop.top);  
	//pDC->LineTo(rcDrop.left+rcDrop.Width()/2,rcDrop.bottom);  
	//pDC->LineTo(rcDrop.left,rcDrop.top);  
	//pDC->EndPath();  
	//pDC->FillPath(); 
}

void CMyComboBox::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}


int CMyComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_dropBrush.CreateSolidBrush(RGB(127,127,127));
	return 0;
}

BOOL CMyComboBox::OnEraseBkgnd(CDC* pDC)
{
	//return true;

 	POINT pt = { 0, 0 };
 	MapWindowPoints(GetParent(), &pt, 1);
 	CPoint orgPt = pDC->OffsetWindowOrg(pt.x, pt.y);
 
 	HRESULT hr =   GetParent()->SendMessage(WM_ERASEBKGND,(WPARAM)(pDC->m_hDC),0);
 
 	pDC->SetWindowOrg(orgPt.x, orgPt.y);

	Draw(pDC);
	return true;
	return CComboBox::OnEraseBkgnd(pDC);
}

void CMyComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	//lpMeasureItemStruct->CtlType = ODT_COMBOBOX;
	// TODO:  添加您的代码以确定指定项的大小
}

void CMyComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMyComboBox::OnCbnSelchange()
{
	
}

void CMyComboBox::OnCbnSelendok()
{
	GetParentFrame()->SendMessage(WM_COMMAND,ID_GOTO_URL,0);
}
bool CMyComboBox::AddOrChoiceItem(LPCTSTR pszUrl)
{
	int nIndex =FindString(0,pszUrl);
	if (nIndex >= 0)
	{
		SetCurSel(nIndex);
	}
	else
	{
		nIndex = AddString(pszUrl);
		SetCurSel(nIndex);
	}
	return false;
}

