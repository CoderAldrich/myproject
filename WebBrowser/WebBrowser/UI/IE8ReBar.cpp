// IE8ReBar.cpp : 实现文件
//

#include "stdafx.h"
#include "UIPublic.h"
#include "IE8ReBar.h"


// CIE8ReBar

IMPLEMENT_DYNAMIC(CIE8ReBar, CReBar)

CIE8ReBar::CIE8ReBar()
{

}

CIE8ReBar::~CIE8ReBar()
{
}


BEGIN_MESSAGE_MAP(CIE8ReBar, CReBar)
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CIE8ReBar::OnNMCustomdraw)
	//ON_WM_PAINT()
	//ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CIE8ReBar 消息处理程序



void CIE8ReBar::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	LPNMCUSTOMDRAW  lplvcd = (LPNMCUSTOMDRAW)pNMHDR;
	
	return ;
	CDC dc;
	dc.Attach(lplvcd->hdc);
	CRect rcWin;
	this->GetWindowRect(&rcWin);
	dc.FillSolidRect(-3,-3,rcWin.Width()+3,rcWin.Height()+3,RGB(0,0,0));
	dc.Detach();

	*pResult = CDRF_SKIPDEFAULT;
}

void CIE8ReBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	dc.FillSolidRect(0,0,2000,200,RGB(255,255,0));
}

BOOL CIE8ReBar::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rcWin;
	GetWindowRect(&rcWin);
	ScreenToClient(&rcWin);
	pDC->FillSolidRect(rcWin,::GetSysColor(COLOR_BTNFACE) /*RGB(240,240,230)*/);
	
	//pDC->SetBkColor(::GetSysColor(COLOR_BTNFACE));
	return true;
	return CReBar::OnEraseBkgnd(pDC);
}
