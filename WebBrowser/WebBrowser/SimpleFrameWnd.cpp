// SimpleFrameWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "WebBrowser.h"
#include "SimpleFrameWnd.h"


#define new DEBUG_NEW
#define AfxDeferRegisterClass(fClass) AfxEndDeferRegisterClass(fClass)
BOOL AFXAPI AfxEndDeferRegisterClass(LONG fToRegister);
#define AFX_WNDFRAMEORVIEW_REG          0x00008


#ifndef _UNICODE
#define _UNICODE_SUFFIX
#else
#define _UNICODE_SUFFIX _T("u")
#endif

#ifndef _DEBUG
#define _DEBUG_SUFFIX
#else
#define _DEBUG_SUFFIX _T("d")
#endif

#ifdef _AFXDLL
#define _STATIC_SUFFIX
#else
#define _STATIC_SUFFIX _T("s")
#endif

#define AFX_WNDCLASS(s) \
	_T("Afx") _T(s) _T(_MFC_FILENAME_VER) _STATIC_SUFFIX _UNICODE_SUFFIX _DEBUG_SUFFIX

IMPLEMENT_DYNCREATE(CSimpleFrameWnd, CWnd)

BEGIN_MESSAGE_MAP(CSimpleFrameWnd, CWnd)
 	ON_WM_CREATE()
 	ON_WM_PAINT()
 	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimpleFrameWnd construction/destruction

CSimpleFrameWnd::CSimpleFrameWnd()
{

}

CSimpleFrameWnd::~CSimpleFrameWnd()
{

}



BOOL CSimpleFrameWnd::Create(LPCTSTR lpszClassName,
					   LPCTSTR lpszWindowName,
					   DWORD dwStyle,
					   const RECT& rect,
					   CWnd* pParentWnd,
					   DWORD dwExStyle)
{
	HMENU hMenu = NULL;

	DWORD dwTempStyle = ((dwStyle&WS_VISIBLE)?WS_VISIBLE:0);
	if (!CreateEx(dwExStyle, lpszClassName, lpszWindowName,dwTempStyle|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		pParentWnd?pParentWnd->GetSafeHwnd():theApp.m_pMainWnd->m_hWnd //没有提供父窗口的时候用默认的父窗口
		, hMenu, (LPVOID)NULL))
	{
		return FALSE;
	}
	return TRUE;
}


void CSimpleFrameWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

void CSimpleFrameWnd::OnNcPaint()
{
	
}

BOOL CSimpleFrameWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
