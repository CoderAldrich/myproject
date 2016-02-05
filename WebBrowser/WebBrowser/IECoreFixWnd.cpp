// IECoreFixWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "WebBrowser.h"
#include "IECoreFixWnd.h"

// CIECoreFixWnd

IMPLEMENT_DYNAMIC(CIECoreFixWnd, CWnd)

#define IsCTRLpressed() ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

CIECoreFixWnd::CIECoreFixWnd()
{
	m_hWndZoomNotify = NULL;
}

CIECoreFixWnd::~CIECoreFixWnd()
{
}


BEGIN_MESSAGE_MAP(CIECoreFixWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_GETOBJECT,OnGetObject)
	ON_MESSAGE(nHtmlMsg,OnHtmlGetObject)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


VOID CIECoreFixWnd::SetZoomNotifyWindow(HWND hWnd,UINT nMsg)
{
	m_hWndZoomNotify = hWnd;
	m_nZoomMsg = nMsg;
}

LRESULT CIECoreFixWnd::OnGetObject( WPARAM wParam,LPARAM lParam )
{
    DWORD dwObjId = (DWORD) lParam;

    if (OBJID_WINDOW==dwObjId)
    {
        return S_OK;
    }
    return CWnd::OnGetObject(wParam,lParam);
}

LRESULT CIECoreFixWnd::OnHtmlGetObject( WPARAM wParam,LPARAM lParam )
{
	return 0;
}

// CIECoreFixWnd 消息处理程序

BOOL CIECoreFixWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_NUMPAD0 ))
	{
		if (IsCTRLpressed())
		{
			if (m_hWndZoomNotify && ::IsWindow(m_hWndZoomNotify))
			{
				::PostMessage(m_hWndZoomNotify,m_nZoomMsg,0,100);
			}
		}
	}
    return CWnd::PreTranslateMessage(pMsg);
}

BOOL CIECoreFixWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

BOOL CIECoreFixWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (IsCTRLpressed())
	{
		if (m_hWndZoomNotify && ::IsWindow(m_hWndZoomNotify))
		{
			::PostMessage(m_hWndZoomNotify,m_nZoomMsg,zDelta,0);
		}
	}
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CIECoreFixWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return CWnd::OnKeyDown(nChar,nRepCnt,nFlags);
}