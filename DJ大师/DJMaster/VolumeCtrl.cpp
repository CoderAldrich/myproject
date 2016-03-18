// VolumeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "DJMaster.h"
#include "VolumeCtrl.h"


// CVolumeCtrl

IMPLEMENT_DYNAMIC(CVolumeCtrl, CWnd)

CVolumeCtrl::CVolumeCtrl()
{
	m_uCurVolume = 0;
	m_hWndNotify = NULL;
	m_uMsgNotify = 0;
}

CVolumeCtrl::~CVolumeCtrl()
{
}


BEGIN_MESSAGE_MAP(CVolumeCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()



// CVolumeCtrl 消息处理程序
VOID CVolumeCtrl::SetNotifyParam(HWND hWndNotify,UINT uMsgNotify)
{
	m_hWndNotify = hWndNotify;
	m_uMsgNotify = uMsgNotify;
}

VOID CVolumeCtrl::InitStatus(UINT uInitVolume)
{
	m_uCurVolume = uInitVolume;

	UpdateStaticShow();
	UpdateSpliderShow();
}
UINT CVolumeCtrl::GetVolumeValue()
{
	return m_uCurVolume;
}
VOID CVolumeCtrl::UpdateStaticShow()
{
	CString strStaticText;
	strStaticText.Format(L"%u",m_uCurVolume);
	m_wndStatic.SetWindowText(strStaticText);
}

VOID CVolumeCtrl::UpdateSpliderShow()
{
	m_wndVolume.SetPos(1000-m_uCurVolume);
}

VOID CVolumeCtrl::NotifyChange()
{
	if (m_hWndNotify)
	{
		::PostMessage(m_hWndNotify,m_uMsgNotify,m_uCurVolume,0);
	}
}



BOOL CVolumeCtrl::OnEraseBkgnd(CDC* pDC)
{
	CPoint pt;
	CRect rcClip;
	pDC->GetClipBox(rcClip);
	pt = rcClip.TopLeft();
	MapWindowPoints(GetParent(), &pt, 1);
	CPoint orgPt = pDC->OffsetWindowOrg(pt.x, pt.y);

	GetParent()->SendMessage(WM_ERASEBKGND,(WPARAM)(pDC->m_hDC),0);

	pDC->SetWindowOrg(orgPt.x, orgPt.y);

	return TRUE;
}

void CVolumeCtrl::OnPaint()
{
	CPaintDC dc(this);
}
int CVolumeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	
	m_wndVolume.Create(WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|/*TBS_AUTOTICKS |*/ TBS_VERT/*|TBS_TOOLTIPS*/ /*| TBS_BOTH*/ ,CRect(0,0,10,10),this,0);
	m_wndVolume.SetRange(0,1000);

	m_wndStatic.Create(L"",WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|SS_CENTER,CRect(0,0,10,10),this);
	CFont font;
	font.Attach(GetStockObject(DEFAULT_GUI_FONT));
	m_wndStatic.SetFont(&font,FALSE);

	RelayoutChild(lpCreateStruct->cx,lpCreateStruct->cy);

	UpdateStaticShow();

	return 0;
}

void CVolumeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	RelayoutChild(cx,cy);
}

VOID CVolumeCtrl::RelayoutChild(int nX, int nY)
{
	if (::IsWindow(m_wndVolume.m_hWnd))
	{
		CRect rcVolume;
		rcVolume.top = 20;
		rcVolume.left = 0;
		rcVolume.right = nX;
		rcVolume.bottom = nY;

		rcVolume.DeflateRect((nX-30)/2,3,(nX-30)/2,3);
		m_wndVolume.MoveWindow(rcVolume);
	}

	if (::IsWindow(m_wndStatic.m_hWnd))
	{
		CRect rcStatic;
		rcStatic.top = 0;
		rcStatic.left = 0;
		rcStatic.right = nX;
		rcStatic.bottom = rcStatic.top+20;

		rcStatic.DeflateRect(3,3,3,3);
		m_wndStatic.MoveWindow(rcStatic);
	}

	return VOID();
}

void CVolumeCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( SB_THUMBTRACK == nSBCode )
	{
		if((PVOID)pScrollBar == (PVOID)&m_wndVolume)
		{
			m_uCurVolume = 1000-nPos;

			//更新 Label显示
			UpdateStaticShow();

			//通知状态发生变化
			NotifyChange();
		}
		
	}
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}
