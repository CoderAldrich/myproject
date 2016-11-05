// BaiDuSEOView.cpp : 实现文件
//

#include "stdafx.h"
#include "BaiDuSEO.h"
#include "BaiDuSEOView.h"

#include "MainFrm.h"

// CBaiDuSEOView

IMPLEMENT_DYNCREATE(CBaiDuSEOView, CIECoreView)

CBaiDuSEOView::CBaiDuSEOView()
{

}

CBaiDuSEOView::~CBaiDuSEOView()
{
}

void CBaiDuSEOView::DoDataExchange(CDataExchange* pDX)
{
	CIECoreView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBaiDuSEOView, CIECoreView)
END_MESSAGE_MAP()


// CBaiDuSEOView 诊断

#ifdef _DEBUG
void CBaiDuSEOView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CBaiDuSEOView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG


BOOL CBaiDuSEOView::WaitDocumentComplete(DWORD dwTimeOut)
{
	BOOL bComplete = FALSE;
	for ( int i=0;i< dwTimeOut;i++ )
	{
		if (READYSTATE_COMPLETE == GetReadyState())
		{
			bComplete = TRUE;
			break;
		}
		Sleep(1);
	}

	return bComplete;
}

void CBaiDuSEOView::NewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl)
{
	CMainFrame *pMainFrame = new CMainFrame;
	pMainFrame->CreateInstance();
	*ppDisp = pMainFrame->m_pView->GetApplication();
}
