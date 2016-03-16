// VolumeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "DJMaster.h"
#include "VolumeCtrl.h"


// CVolumeCtrl

IMPLEMENT_DYNAMIC(CVolumeCtrl, CWnd)

CVolumeCtrl::CVolumeCtrl()
{
	m_uPos = 0;
	m_uMinValue = 0;
	m_uMaxValue = 100;
}

CVolumeCtrl::~CVolumeCtrl()
{
}


BEGIN_MESSAGE_MAP(CVolumeCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CVolumeCtrl 消息处理程序



BOOL CVolumeCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CVolumeCtrl::OnPaint()
{
	CPaintDC dc(this);

	CRect rcClient;
	GetClientRect(&rcClient);
	dc.Rectangle(&rcClient);


	CRect rcCtrlBlock;

	CalcCtrlBlockRect(rcCtrlBlock);
	dc.Rectangle(&rcCtrlBlock);
	rcCtrlBlock.DeflateRect(1,1,1,1);
	dc.FillSolidRect(rcCtrlBlock,RGB(230,230,230));

}

VOID CVolumeCtrl::CalcCtrlBlockRect(CRect & rcCtrlBlock)
{
	GetClientRect(&rcCtrlBlock);

	rcCtrlBlock.DeflateRect(3,3,3,3);

	rcCtrlBlock.top += m_uPos-15;
	rcCtrlBlock.bottom = rcCtrlBlock.top+15;

	rcCtrlBlock = rcCtrlBlock;
}

VOID CVolumeCtrl::SetValueRange(UINT uMinValue, UINT uMaxValue)
{
	m_uMinValue = uMinValue;
	m_uMaxValue = uMaxValue;
	return VOID();
}

void CVolumeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetTimer(1000,10,NULL);
	CWnd::OnLButtonDown(nFlags, point);
}

void CVolumeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	KillTimer(1000);
	CWnd::OnLButtonUp(nFlags, point);
}

void CVolumeCtrl::OnTimer(UINT_PTR nIDEvent)
{
	if ( GetAsyncKeyState(VK_LBUTTON) )
	{
		CPoint ptMouse;
		GetCursorPos(&ptMouse);
		ScreenToClient(&ptMouse);

		if ( m_uPos != ptMouse.y )
		{
			m_uPos = ptMouse.y;
			CRect rcClient;
			GetClientRect(&rcClient);
			InvalidateRect(&rcClient);
		}
	}
	else
	{
		KillTimer(nIDEvent);
	}


	CWnd::OnTimer(nIDEvent);
}
