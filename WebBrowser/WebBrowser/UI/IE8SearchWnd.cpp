// IE8SearchEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "..\resource.h"
#include "UIPublic.h"
#include "IE8SearchWnd.h"


// CIE8SearchEdit

IMPLEMENT_DYNAMIC(CIE8SearchWnd, CControlBar)

CIE8SearchWnd::CIE8SearchWnd()
{
	m_defaultfont.Attach((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	m_hicoSearch = LoadIconW(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_IE9_SEARCH));
	int a=0;
}

CIE8SearchWnd::~CIE8SearchWnd()
{
}


BEGIN_MESSAGE_MAP(CIE8SearchWnd, CControlBar)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_MESSAGE(WM_EDIT_RETURN_MSG,OnEditReturn)
END_MESSAGE_MAP()



// CIE8SearchEdit 消息处理程序

LRESULT CIE8SearchWnd::OnEditReturn(WPARAM wParam,LPARAM lParam)
{
	CWnd *pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd)
	{
		pFrameWnd->PostMessage(WM_COMMAND,ID_SEARCH,0);
	}
	return 0;
}

CString CIE8SearchWnd::GetSearchText(void)
{
	CString strText;
	m_editSearch.GetWindowText(strText);
	return strText;
}


int CIE8SearchWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_editSearch.Create(WS_VISIBLE|WS_CHILDWINDOW,CRect(0,0,12,12),this,0);
	m_editSearch.SetFont(&m_defaultfont);
	m_editSearch.SetNotifyMessageOnReturn(WM_EDIT_RETURN_MSG);

	m_tbSearch.CreateEx(this,TBSTYLE_FLAT|TBSTYLE_LIST|TBSTYLE_TRANSPARENT|TBSTYLE_AUTOSIZE|CCS_NODIVIDER|CCS_NOPARENTALIGN|CCS_NORESIZE);

	m_tbSearch.AddButton(0,ID_SEARCH,TBBS_BUTTON,TBSTATE_ENABLED,0);
	m_tbSearch.AddButton(1,ID_NOTHING_ENABLE,TBBS_BUTTON,TBSTATE_ENABLED,1);

	const UINT bmpWidth = 22;
	const UINT bmpHeight = 22;
	m_tbSearch.SetImageList(IDB_SEARCH_XP  ,bmpWidth,bmpHeight);
	m_tbSearch.SetHotImageList(IDB_SEARCH_HOT_XP  ,bmpWidth,bmpHeight);
	m_tbSearch.SetPressImageList(IDB_SEARCH_PRESSED  ,bmpWidth,bmpHeight);


 	m_tbSearch.SetSizes(CSize(22,29),CSize(14,22));

	m_tbSearch.SetButtonSpace(0);
	m_tbSearch.SetButtonPad(0,0);

	return 0;
}

BOOL CIE8SearchWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

 	POINT pt = { 0, 0 };
 	MapWindowPoints(GetParent(), &pt, 1);
 	CPoint orgPt = pDC->OffsetWindowOrg(pt.x, pt.y);
 
 	HRESULT hr =   GetParent()->SendMessage(WM_ERASEBKGND,(WPARAM)(pDC->m_hDC),0);
 
 	pDC->SetWindowOrg(orgPt.x, orgPt.y);


	pDC->FillSolidRect(m_rcInput,RGB(255,255,255));
	//pDC->Draw3dRect(m_rcInput,RGB(127,157,186),RGB(127,157,186));

	//win7下风格
	if( ::GetFocus() == m_editSearch.m_hWnd)
	{
		pDC->Draw3dRect(m_rcInput,RGB(61,123,173),RGB(183,217,237));
	}
	else
	{
		pDC->Draw3dRect(m_rcInput,RGB(171,173,179),RGB(227,233,239));
	}

	CRect rcWin;
	GetWindowRect(&rcWin);

	static const UINT nSize = 18;
	int nleft= (m_rcInput.Height() - nSize)/2+m_rcInput.left;
	int ntop = (m_rcInput.Height() - nSize)/2+m_rcInput.top;
	DrawIconEx(pDC->m_hDC,nleft,ntop,m_hicoSearch,nSize,nSize,0,NULL,DI_NORMAL);


	return true;
	//return CWnd::OnEraseBkgnd(pDC);
}

void CIE8SearchWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CRect rcLast;
	m_tbSearch.GetItemRect(m_tbSearch.GetToolBarCtrl().GetButtonCount()-1,&rcLast);
	m_tbSearch.MoveWindow(cx-rcLast.right+5,0,rcLast.right,cy);


	static const UINT nEdgeHeight = 22;
	m_rcInput.left = 0;
	m_rcInput.top = (cy-nEdgeHeight)/2;
	m_rcInput.bottom = (cy-nEdgeHeight)/2+nEdgeHeight;
	m_rcInput.right = cx-rcLast.right;

	//23 
	static const UINT nEditHeight = 12;
	m_editSearch.MoveWindow(nEdgeHeight+1,(cy-nEditHeight)/2,cx-rcLast.right-5-(nEdgeHeight+2),nEditHeight);

	
}

void CIE8SearchWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
}
