// Hao123View.cpp : 实现文件
//

#include "stdafx.h"
#include "BaiduResultView.h"
#include "AutoBrowser.h"
#include "MainFrm.h"

IMPLEMENT_DYNCREATE(CBaiduResultView, CAutoBrowserView)

CBaiduResultView::CBaiduResultView()
{
	m_bClickElem = FALSE;
	m_nClickElemCount = 0;
	m_nMaxClickElemCount = 0;

}

CBaiduResultView::~CBaiduResultView()
{
}

void CBaiduResultView::DoDataExchange(CDataExchange* pDX)
{
	CAutoBrowserView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBaiduResultView, CAutoBrowserView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBaiduResultView 诊断

#ifdef _DEBUG
void CBaiduResultView::AssertValid() const
{
	CAutoBrowserView::AssertValid();
}

void CBaiduResultView::Dump(CDumpContext& dc) const
{
	CAutoBrowserView::Dump(dc);
}
#endif //_DEBUG


VOID CBaiduResultView::SetWorkParam(UINT nMaxClickElemCount)
{
	m_nMaxClickElemCount = nMaxClickElemCount;
}

BOOL CBaiduResultView::StartWork()
{
	SetTimer(CLICK_BAIDU_SEARCH_RESULT_TIME_ID,4000,NULL);
	return TRUE;
}
void CBaiduResultView::OnNewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl)
{
	m_bClickElem = TRUE;

	CMainFrame *pMainFrame = new CMainFrame;
	*ppDisp = pMainFrame->CreateInstance(TYPE_PC_AUTO_CLOSE_VIEW);

	m_nClickElemCount++;
}
void CBaiduResultView::OnTimer(UINT_PTR nIDEvent)
{
	if ( CLICK_BAIDU_SEARCH_RESULT_TIME_ID == nIDEvent )
	{
		KillTimer(nIDEvent);
		

		//超过了最大点击次数
		if (m_nClickElemCount >= m_nMaxClickElemCount)
		{
			return;
		}

		CAutoBrowser AutoBrowser((IWebBrowser2 *)GetApplication(),GetIEServerWnd());

		BOOL bClickPopularizeRes = FALSE;
		if( CAutoBrowser::GetRandValue(0,100) < 80 )
		{
			CElementInformation ElemInfo;
			ElemInfo.SetTagName(L"a");
			ElemInfo.AddElementAttribute(L"href",L"www.baidu.com/baidu.php",FALSE);

			//随机点击推广链接
			bClickPopularizeRes = AutoBrowser.ClickRandMatchWebPageElement(&ElemInfo);
#ifdef DEBUG
			if ( FALSE == bClickPopularizeRes)
			{
				OutputDebugStringW(L"点击推广链接失败\n");
			}
#endif
		}

		if ( bClickPopularizeRes == FALSE )
		{

#ifdef DEBUG
			OutputDebugStringW(L"点击普通链接\n");
#endif

			CElementInformation ElemInfo;
			ElemInfo.SetTagName(L"a");
			ElemInfo.AddElementAttribute(L"href",L"www.baidu.com/link",FALSE);
			AutoBrowser.ClickRandMatchWebPageElement(&ElemInfo);
		}
		
		SetTimer(CLICK_BAIDU_SEARCH_RESULT_TIME_ID,3000,NULL);

	}


	CAutoBrowserView::OnTimer(nIDEvent);
}
