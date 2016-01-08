// ADWindowMgrView.cpp : CAutoBrowserView 类的实现
//

#include "stdafx.h"
#include "MainFrm.h"
#include "AutoBrowserView.h"
#include "AutoBrowser.h"
#include "VirtualMouse.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoBrowserView

IMPLEMENT_DYNCREATE(CAutoBrowserView, CIECoreView)

BEGIN_MESSAGE_MAP(CAutoBrowserView, CIECoreView)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CAutoBrowserView,  CIECoreView)
END_EVENTSINK_MAP()
// CAutoBrowserView 构造/析构

CAutoBrowserView::CAutoBrowserView()
{
	m_bVirtualMoveing = FALSE;
	m_nVirtualMove = 0;
	m_nMaxVirtualMove = 0;
	m_nAfterMoveTimeID = 0;
	m_nAfterTimeDelay = 0;
	m_bAutoStartWork = FALSE;
	m_bAutoStartWorkDo = FALSE;

}

CAutoBrowserView::~CAutoBrowserView()
{
	
}

void CAutoBrowserView::OnNewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl)
{
	*Cancel = VARIANT_TRUE;
}
void CAutoBrowserView::NewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl)
{
	OnNewWindow3(ppDisp,Cancel,dwFlags,bstrUrlContext,bstrUrl);
}
// CAutoBrowserView 诊断

#ifdef _DEBUG
void CAutoBrowserView::AssertValid() const
{
	CIECoreView::AssertValid();
}

void CAutoBrowserView::Dump(CDumpContext& dc) const
{
	CIECoreView::Dump(dc);
}

#endif //_DEBUG




void CAutoBrowserView::OnMainDocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	__super::OnMainDocumentComplete(pDisp , URL);
	if( FALSE == m_bAutoStartWorkDo )
	{
		m_bAutoStartWorkDo = TRUE;

		if (m_bAutoStartWork)
		{
			StartWork();
		}
	}
}

void CAutoBrowserView::OnTimer(UINT_PTR nIDEvent)
{
	CIECoreView::OnTimer(nIDEvent);
}



BOOL CAutoBrowserView::StartWork()
{
	return FALSE;
}
VOID CAutoBrowserView::AutoStartWork()
{
	m_bAutoStartWork = TRUE;
}