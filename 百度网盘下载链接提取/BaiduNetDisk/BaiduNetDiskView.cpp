// Hao123View.cpp : 实现文件
//

#include "stdafx.h"
#include "BaiduNetDiskView.h"
#include "AutoBrowser.h"
#include "MainFrm.h"
#include "VirtualMouse.h"
#include "PublicFun.h"
#include "SAStatusLog.h"

#include <math.h>

extern CSAStatusLog g_Loger;

extern CString g_strCheckCode;
extern CString g_strResSave;
IMPLEMENT_DYNCREATE(CBaiduNetDiskView, CAutoBrowserView)

CBaiduNetDiskView::CBaiduNetDiskView()
{
	m_nMoveCount = 0;
	m_bClickSuccess = FALSE;
	m_nClickTryTimes = 0;
}

CBaiduNetDiskView::~CBaiduNetDiskView()
{
}

void CBaiduNetDiskView::DoDataExchange(CDataExchange* pDX)
{
	CAutoBrowserView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBaiduNetDiskView, CAutoBrowserView)
	ON_WM_TIMER()
END_MESSAGE_MAP()



#ifdef _DEBUG
void CBaiduNetDiskView::AssertValid() const
{
	CAutoBrowserView::AssertValid();
}

void CBaiduNetDiskView::Dump(CDumpContext& dc) const
{
	CAutoBrowserView::Dump(dc);
}
#endif //_DEBUG


void CBaiduNetDiskView::NewWindow3( IDispatch **ppDisp,VARIANT_BOOL *Cancel,DWORD dwFlags,BSTR bstrUrlContext,BSTR bstrUrl)
{
	if (!ppDisp)
		return;
	*Cancel = VARIANT_TRUE;
}

#define INPUT_CODE 1000
#define CLICK_CODE 1001
#define CLICK_DOWNLLOAD_BTN 1002
#define CLICK_NORMAL_DOWNLLOAD 1003
BOOL CBaiduNetDiskView::StartWork()
{
	SetTimer(INPUT_CODE,1000,NULL);
	return TRUE;
}



void CBaiduNetDiskView::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);

	CAutoBrowser AutoBrowser((IWebBrowser2 *)GetApplication(),GetIEServerWnd());

	//输入提取密码
	if ( INPUT_CODE == nIDEvent )
	{
		AutoBrowser.InputText(g_strCheckCode);

		SetTimer(CLICK_CODE,1000,NULL);
	}

	//点击提交按钮
	if (CLICK_CODE == nIDEvent)
	{
		static int nsubmitClickCount = 0;
		CElementInformation ElemInfo;
		ElemInfo.SetTagName(L"a");
		ElemInfo.AddElementAttribute(L"id",L"submitBtn",TRUE);
		
		if( AutoBrowser.ClickFirstMatchWebPageElement(&ElemInfo) || nsubmitClickCount >= 5  )
		{
			SetTimer(CLICK_DOWNLLOAD_BTN,3000,NULL);
		}
		else
		{
			SetTimer(CLICK_CODE,1000,NULL);
		}

		nsubmitClickCount++;
		
	}


	//点击下载按钮
	if (CLICK_DOWNLLOAD_BTN == nIDEvent)
	{
		static int ndownloadClickCount = 0;
		CElementInformation ElemInfo;
		ElemInfo.SetTagName(L"a");
		ElemInfo.AddElementAttribute(L"id",L"downFileButton",TRUE);
		ElemInfo.AddElementAttribute(L"class",L"new-dbtn",TRUE);
		
		if(AutoBrowser.ClickFirstMatchWebPageElement(&ElemInfo) || ndownloadClickCount >= 5 )
		{
			SetTimer(CLICK_NORMAL_DOWNLLOAD,3000,NULL);
		}
		else
		{
			SetTimer(CLICK_DOWNLLOAD_BTN,3000,NULL);
		}

		ndownloadClickCount++;
		

	}

	//点击普通下载按钮
	if( CLICK_NORMAL_DOWNLLOAD == nIDEvent )
	{
		CElementInformation ElemInfo;
		ElemInfo.SetTagName(L"a");
		ElemInfo.AddElementAttribute(L"id",L"_disk_id_15",TRUE);
		ElemInfo.AddElementAttribute(L"class",L"abtn cancel",TRUE);
		
		if( FALSE == AutoBrowser.ClickFirstMatchWebPageElement(&ElemInfo) )
		{
			SetTimer(CLICK_NORMAL_DOWNLLOAD,3000,NULL);
		}
	}
}


HRESULT CBaiduNetDiskView::OnDownloadFile( BSTR bstrFileUrl )
{
	//文件下载通知


	DeleteFile(g_strResSave);

	CFile ResFile;
	if(ResFile.Open(g_strResSave,CFile::modeCreate|CFile::modeWrite))
	{
		CStringA strFileUrl;
		strFileUrl = bstrFileUrl;
		ResFile.Write(strFileUrl.GetBuffer(),strFileUrl.GetLength());
		ResFile.Close();
	}

	ExitProcess(0);
	return S_OK;
}