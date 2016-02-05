// IE8NaviBar.cpp : 实现文件
//

#include "stdafx.h"
#include "UIPublic.h"
#include "IE8NaviBar.h"

// CIE8NaviBar

IMPLEMENT_DYNAMIC(CIE8NaviBar, CEasyToolBar)

CIE8NaviBar::CIE8NaviBar()
{

}

CIE8NaviBar::~CIE8NaviBar()
{

}


BEGIN_MESSAGE_MAP(CIE8NaviBar, CEasyToolBar)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TBN_DROPDOWN, &CIE8NaviBar::OnTbnDropDown)
	ON_NOTIFY_REFLECT(TBN_HOTITEMCHANGE, &CIE8NaviBar::OnTbnHotItemChange)
	ON_WM_MOUSEMOVE()
	//ON_WM_PAINT()
END_MESSAGE_MAP()



// CIE8NaviBar 消息处理程序



int CIE8NaviBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;


	SetImageList(IDB_IE8_NAVI,25,25);
	SetHotImageList(IDB_IE8_NAVI_HOT,25,25);
	SetPressImageList(IDB_IE8_NAVI_PRESS,25,25);
	SetDisableImageList( IDB_IE8_NAVI_DISABLE,25,25 );

	AddButton(0,ID_GO_BACK,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,0);
	AddButton(1,ID_GO_FORWARD,TBBS_BUTTON|TBBS_AUTOSIZE,TBSTATE_ENABLED,1);

	SetSizes(CSize(29,29),CSize(22,22));

	CRect rcLastItem;
	GetItemRect(1,&rcLastItem);
	m_wndDropDown.Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(rcLastItem.right-6,rcLastItem.top+3,rcLastItem.right+20,rcLastItem.bottom),this,0);

	return 0;
}

void CIE8NaviBar::OnTbnDropDown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTOOLBAR pNMTB = reinterpret_cast<LPNMTOOLBAR>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CIE8NaviBar::OnTbnHotItemChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTBHOTITEM pNMTBHI = reinterpret_cast<LPNMTBHOTITEM>(pNMHDR);

	if (pNMTBHI->idNew == ID_FILE_OPEN)
	{
		CRect rcItem;
		GetItemRect(1,&rcItem);

		CPoint point;

		pNMTBHI->idNew = 0;
		//*pResult = CDRF_SKIPDEFAULT;
		return ;
	}
	*pResult = 0;
}

void CIE8NaviBar::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CToolBar::OnMouseMove(nFlags, point);
}

void CIE8NaviBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CToolBar::OnPaint()
}

