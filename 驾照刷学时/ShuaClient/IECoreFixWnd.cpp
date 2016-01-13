// IECoreFixWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "IECoreFixWnd.h"

// CIECoreFixWnd


static UINT nMsg = ::RegisterWindowMessage( L"WM_HTML_GETOBJECT" );

IMPLEMENT_DYNAMIC(CIECoreFixWnd, CWnd)

CIECoreFixWnd::CIECoreFixWnd()
{

}

CIECoreFixWnd::~CIECoreFixWnd()
{
}


BEGIN_MESSAGE_MAP(CIECoreFixWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_GETOBJECT,OnGetObject)
	ON_MESSAGE(49862,OnHtmlGetObject)
END_MESSAGE_MAP()

LRESULT CIECoreFixWnd::OnGetObject( WPARAM wParam,LPARAM lParam )
{
	return 0;
}

LRESULT CIECoreFixWnd::OnHtmlGetObject( WPARAM wParam,LPARAM lParam )
{
	return 0;
}

// CIECoreFixWnd 消息处理程序

BOOL CIECoreFixWnd::PreTranslateMessage(MSG* pMsg)
{
    return CWnd::PreTranslateMessage(pMsg);
}

BOOL CIECoreFixWnd::OnEraseBkgnd(CDC* pDC)
{
	return true;
	//return CWnd::OnEraseBkgnd(pDC);
}
