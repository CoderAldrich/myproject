// SXSView.cpp : 实现文件
//

#include "stdafx.h"
#include "SXSClient.h"
#include "SXSView.h"
#include "浏览器自动化/AutoBrowser.h"
#include "MainFrm.h"

// CSXSView

IMPLEMENT_DYNCREATE(CSXSView, CIECoreView)

CSXSView::CSXSView()
{

}

CSXSView::~CSXSView()
{
}

void CSXSView::DoDataExchange(CDataExchange* pDX)
{
	CIECoreView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSXSView, CIECoreView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSXSView 诊断

#ifdef _DEBUG
void CSXSView::AssertValid() const
{
	CIECoreView::AssertValid();
}

void CSXSView::Dump(CDumpContext& dc) const
{
	CIECoreView::Dump(dc);
}
#endif //_DEBUG


void CSXSView::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	if ( GetApplication() == pDisp )
	{
		CString strUrl;
		strUrl = URL->bstrVal;
		if ( strUrl.CompareNoCase(L"http://www.130100.prcjx.cn/") == 0 )
		{
			SetTimer(WM_USER+1111,2000,NULL);
		}
		else if( strUrl.CompareNoCase(L"http://www.130100.prcjx.cn:800/admin/std") == 0 )
		{
			SetTimer(WM_USER+1113,2000,NULL);
		}
		else if ( strUrl.Find(L"http://www.130100.prcjx.cn:800/admin/std/training") >= 0 )
		{
			SetTimer(WM_USER+1112,2000,NULL);
		}
		else 
		{
			KillTimer(WM_USER+1112);
		}
		


	}
}


void CSXSView::OnTimer(UINT_PTR nIDEvent)
{
	
	CAutoBrowser AutoBrowser((IWebBrowser2 *)GetApplication(),GetIEServerWnd());

	if (nIDEvent == WM_USER+1111)
	{
		KillTimer(nIDEvent);

		//输入用户名
		{
			CElementInformation ElemInfo;
			ElemInfo.SetTagName(L"input");
			ElemInfo.AddElementAttribute(L"id",L"edit-name",TRUE);

			CElemRectList ElemList;
			AutoBrowser.GetAllMatchElemRect(&ElemList,&ElemInfo);

			if (ElemList.GetElemRectCount() == 1)
			{
				ELEM_RECT ElemRect;
				ElemList.GetElemRectByIndex(0,&ElemRect);

				CComQIPtr<IHTMLInputElement> pInput;
				ElemRect.pElem->QueryInterface(IID_IHTMLInputElement,(void **)&pInput);
				pInput->put_value(theApp.m_strUserName.AllocSysString());
			}
		}

		//输入密码
		{
			CElementInformation ElemInfo;
			ElemInfo.SetTagName(L"input");
			ElemInfo.AddElementAttribute(L"id",L"edit-pass",TRUE);


			CElemRectList ElemList;
			AutoBrowser.GetAllMatchElemRect(&ElemList,&ElemInfo);

			if (ElemList.GetElemRectCount() == 1)
			{
				ELEM_RECT ElemRect;
				ElemList.GetElemRectByIndex(0,&ElemRect);

				CComQIPtr<IHTMLInputElement> pInput;
				ElemRect.pElem->QueryInterface(IID_IHTMLInputElement,(void **)&pInput);
				pInput->put_value(theApp.m_strPassWord.AllocSysString());
			}
		}

		//选中验证码框
		{
			CElementInformation ElemInfo;
			ElemInfo.SetTagName(L"input");
			ElemInfo.AddElementAttribute(L"id",L"edit-captcha-response",TRUE);
			AutoBrowser.ClickFirstMatchWebPageElement(&ElemInfo);
		}

	}
	
	if (nIDEvent == WM_USER+1112)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		AutoBrowser.SetWebPageMousePos(CAutoBrowser::GetRandValue(0,rcClient.Width()),CAutoBrowser::GetRandValue(0,rcClient.Height()));
	}


	if ( nIDEvent == WM_USER+1113 )
	{
		KillTimer(nIDEvent);

		CString strUserRealName;


		CElementInformation ElemInfo;
		ElemInfo.SetTagName(L"a");
		ElemInfo.AddElementAttribute(L"href",L"/admin/std/info",TRUE);

		CElementInformation *pParentInfo = ElemInfo.CreateParentInfo();
		pParentInfo->SetTagName(L"span");
		pParentInfo->AddElementAttribute(L"class",L"user-title",TRUE);

		CElemRectList ElemList;
		AutoBrowser.GetAllMatchElemRect(&ElemList,&ElemInfo);

		if (ElemList.GetElemRectCount() == 1)
		{
			ELEM_RECT ElemRect;
			ElemList.GetElemRectByIndex(0,&ElemRect);

			CComQIPtr<IHTMLElement> pElem;
			ElemRect.pElem->QueryInterface(IID_IHTMLElement,(void **)&pElem);
			if (pElem)
			{
				CComBSTR bstrInnerText;
				pElem->get_innerText(&bstrInnerText);
				strUserRealName = bstrInnerText;
			}
		}

		CMainFrame *pParentFrame = (CMainFrame *)GetParent();
		if (pParentFrame)
		{
			pParentFrame->UpdateFrameTitle(theApp.m_strUserName+L" "+strUserRealName);
		}
		


	}


	CIECoreView::OnTimer(nIDEvent);
}
