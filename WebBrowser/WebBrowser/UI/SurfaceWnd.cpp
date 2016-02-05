#include "stdafx.h"
#include "SurfaceWnd.h"

BEGIN_MESSAGE_MAP(CSurfaceWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

int CSurfaceWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CSurfaceWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE | TME_HOVER;//要触发的消息
		tme.hwndTrack = this->m_hWnd;
		tme.dwHoverTime = 10;// 若不设此参数，则无法触发mouseHover

		if (::_TrackMouseEvent(&tme)) //MOUSELEAVE|MOUSEHOVER消息由此函数触发
		{
			m_bTracking = true;   
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CSurfaceWnd::OnMouseHover(UINT nFlags, CPoint point)
{
	m_bTracking = true;
	::SendMessage(m_hWndNotify,m_nMsgNotify,2,0);
	CWnd::OnMouseHover(nFlags, point);
}

void CSurfaceWnd::OnMouseLeave()
{
	m_bMouseDown = false;
	m_bTracking = false;
	::SendMessage(m_hWndNotify,m_nMsgNotify,1,0);
	CWnd::OnMouseLeave();
}

void CSurfaceWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseDown = false;
	::SendMessage(m_hWndNotify,m_nMsgNotify,0,0);

	CWnd::OnLButtonUp(nFlags, point);
}

void CSurfaceWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseDown = true; 
	::SendMessage(m_hWndNotify,m_nMsgNotify,3,0);
	CWnd::OnLButtonDown(nFlags, point);
}
void CSurfaceWnd::RegisterStatusChangeNotify(HWND hWnd,UINT nMsg)
{
	m_hWndNotify =hWnd;
	m_nMsgNotify =nMsg;
}

BOOL CSurfaceWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	cs.dwExStyle|=WS_EX_TRANSPARENT;
	return CWnd::PreCreateWindow(cs);
}
