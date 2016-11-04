// IECoreFixWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "IECoreFixWnd.h"

// CIECoreFixWnd

IMPLEMENT_DYNAMIC(CIECoreFixWnd, CWnd)

CIECoreFixWnd::CIECoreFixWnd()
{

}

CIECoreFixWnd::~CIECoreFixWnd()
{

}


BEGIN_MESSAGE_MAP(CIECoreFixWnd, CWnd)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL CIECoreFixWnd::OnEraseBkgnd(CDC* pDC)
{
	return true;
	//return CWnd::OnEraseBkgnd(pDC);
}
