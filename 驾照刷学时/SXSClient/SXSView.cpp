// SXSView.cpp : 实现文件
//

#include "stdafx.h"
#include "SXSClient.h"
#include "SXSView.h"
#include "浏览器自动化/AutoBrowser.h"
#include "MainFrm.h"
#include "PauseMonitor.h"
#include <detours.h>
#include "SAStatusLog.h"

CSAStatusLog g_loger(L"sxslog");

#define TIME_ID_INPUT_LOGIN       1000
#define TIME_ID_QUERY_USER_INFO   1001
#define TIME_ID_MOUSE_MOVE        1002
#define TIME_ID_CHECK_VIDEO_PAUSE 1003
#define TIME_ID_CLICK_RESUME      1004
#define TIME_ID_CLICK_WATCH_CONTINE 1005

#define WM_ON_VIDEO_PAUSE          WM_USER+1000
#define WM_ON_VIDEO_RESUME         WM_USER+1001
// CSXSView

IMPLEMENT_DYNCREATE(CSXSView, CIECoreView)


int (WINAPI *pMessageBoxW)(
						   __in_opt HWND hWnd,
						   __in_opt LPCWSTR lpText,
						   __in_opt LPCWSTR lpCaption,
						   __in UINT uType
						   ) = MessageBoxW;
int WINAPI MyMessageBoxW(
						 __in_opt HWND hWnd,
						 __in_opt LPCWSTR lpText,
						 __in_opt LPCWSTR lpCaption,
						 __in UINT uType
						 )
{

	g_loger.StatusOut(L"对话框拦截 MessageBoxW Caption:%s Text:%s Style:%d",lpCaption,lpText,uType);

	int TReturn = pMessageBoxW(
		hWnd,
		lpText,
		lpCaption,
		uType
		);
	return TReturn;
};

int (WINAPI *pMessageBoxIndirectW)(
								   __in CONST MSGBOXPARAMSW * lpmbp
								   ) = MessageBoxIndirectW;
int WINAPI MyMessageBoxIndirectW(
								 __in CONST MSGBOXPARAMSW * lpmbp
								 )
{
	CString strCaption;
	CString strText;

	strCaption = lpmbp->lpszCaption;
	strText = lpmbp->lpszText;

	
	g_loger.StatusOut(L"对话框拦截 MessageBoxIndirectW Caption:%s Text:%s Style:%d",strCaption,strText,lpmbp->dwStyle);

	if (
		strCaption.CompareNoCase(L"来自网页的消息") == 0 
		&& strText.CompareNoCase(L"您确实要退出理论培训吗?") == 0
		)
	{
		return IDOK;
	}

	int TReturn = pMessageBoxIndirectW(
		lpmbp
		);
	return TReturn;
};


DLGPROC pDlgProc = NULL;

INT_PTR CALLBACK MyDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( WM_COMMAND == uMsg )
	{
		int a=0;
	}
	CString strMsgOut;
	strMsgOut.Format(L"HWND %x Msg %x WPARAM %d LPARAM %d\r\n",hWnd,uMsg,wParam,lParam);
	OutputDebugStringW(strMsgOut);
	return pDlgProc(hWnd, uMsg, wParam, lParam);
}

INT_PTR (WINAPI *pDialogBoxIndirectParamW)(
	__in_opt HINSTANCE hInstance,
	__in LPCDLGTEMPLATEW hDialogTemplate,
	__in_opt HWND hWndParent,
	__in_opt DLGPROC lpDialogFunc,
	__in LPARAM dwInitParam
	) = DialogBoxIndirectParamW;
INT_PTR WINAPI MyDialogBoxIndirectParamW(
	__in_opt HINSTANCE hInstance,
	__in LPCDLGTEMPLATEW hDialogTemplate,
	__in_opt HWND hWndParent,
	__in_opt DLGPROC lpDialogFunc,
	__in LPARAM dwInitParam
	)
{
	if ( NULL == pDlgProc && lpDialogFunc)
	{
		pDlgProc = lpDialogFunc;
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach( (PVOID *)&pDlgProc ,(PVOID)MyDialogProc );
		DetourTransactionCommit();
	}


	INT_PTR TReturn = pDialogBoxIndirectParamW(
		hInstance,
		hDialogTemplate,
		hWndParent,
		lpDialogFunc,
		dwInitParam
		);
	return TReturn;
};

CSXSView::CSXSView()
{
	m_refPreColor = 0;
	m_nColorSameCount = 0;

	static BOOL bHook = FALSE;
	if ( !bHook )
	{
		bHook = TRUE;

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach( (PVOID *)&pMessageBoxW ,(PVOID)MyMessageBoxW );
		DetourAttach( (PVOID *)&pMessageBoxIndirectW ,(PVOID)MyMessageBoxIndirectW );
		DetourAttach( (PVOID *)&pDialogBoxIndirectParamW ,(PVOID)MyDialogBoxIndirectParamW );
		
		DetourTransactionCommit();
	}
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
	ON_MESSAGE(WM_ON_VIDEO_PAUSE,OnVideoPause)
	ON_MESSAGE(WM_ON_VIDEO_RESUME,OnVideoResume)
	ON_WM_CREATE()
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

int CSXSView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CIECoreView::OnCreate(lpCreateStruct) == -1)
		return -1;

#ifndef DEBUG
	StartPauseMonitor(m_hWnd,WM_ON_VIDEO_PAUSE,WM_ON_VIDEO_RESUME);
#endif
	return 0;
}


void CSXSView::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	if ( GetApplication() == pDisp )
	{
		CString strUrl;
		strUrl = URL->bstrVal;
		if ( strUrl.CompareNoCase(L"http://www.130100.prcjx.cn/") == 0 )
		{
			SetTimer(TIME_ID_INPUT_LOGIN,2000,NULL);

			g_loger.StatusOut(L"打开首页");
		}
		else if( strUrl.CompareNoCase(L"http://www.130100.prcjx.cn:800/admin/std") == 0 )
		{
			SetTimer(TIME_ID_QUERY_USER_INFO,2000,NULL);
			SetTimer(TIME_ID_CLICK_WATCH_CONTINE,4000,NULL);
			g_loger.StatusOut(L"打开用户首页");
		}
		
		if ( strUrl.Find(L"http://www.130100.prcjx.cn:800/admin/std/training") >= 0 )
		{
			SetTimer( TIME_ID_MOUSE_MOVE   , 5000,NULL);
			SetTimer( TIME_ID_CHECK_VIDEO_PAUSE , 5000 , NULL );
			g_loger.StatusOut(L"打开视频教程");
		}
		else 
		{
			KillTimer(TIME_ID_MOUSE_MOVE);
		}
	}
}


void CSXSView::OnTimer(UINT_PTR nIDEvent)
{
	
	CAutoBrowser AutoBrowser((IWebBrowser2 *)GetApplication(),GetIEServerWnd());

	if (nIDEvent == TIME_ID_INPUT_LOGIN)
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
	
	if (nIDEvent == TIME_ID_MOUSE_MOVE)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		AutoBrowser.SetWebPageMousePos(CAutoBrowser::GetRandValue(0,rcClient.Width()),CAutoBrowser::GetRandValue(0,rcClient.Height()));
	}


	if ( nIDEvent == TIME_ID_QUERY_USER_INFO )
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
			g_loger.StatusOut(L"获取用户信息：%s %s",theApp.m_strUserName,strUserRealName);
		}

	}

	if( nIDEvent == TIME_ID_CLICK_WATCH_CONTINE )
	{
		KillTimer(nIDEvent);

		CElementInformation ElemInfo;
		ElemInfo.SetTagName(L"a");
		ElemInfo.SetTextName(L"继续观看",FALSE);
		ElemInfo.AddElementAttribute(L"href",L"/admin/std/training/",FALSE);
		AutoBrowser.ClickFirstMatchWebPageElement(&ElemInfo);

		g_loger.StatusOut(L"点击继续学习按钮");

	}

	if ( nIDEvent == TIME_ID_CLICK_RESUME )
	{
		KillTimer(nIDEvent);

		CElementInformation ElemInfo;
		CElemRectList ElemList;

		ElemInfo.SetTagName(L"object");
		ElemInfo.AddElementAttribute(L"id",L"VMSPlayer",TRUE);

		AutoBrowser.GetAllMatchElemRect(&ElemList,&ElemInfo);
		if ( ElemList.GetElemRectCount() == 1 )
		{
			// 51 382
			ELEM_RECT ElemRect;
			ElemList.GetElemRectByIndex(0,&ElemRect);
			AutoBrowser.ClickWebPagePoint(ElemRect.rcElem.left+51,ElemRect.rcElem.top+382);
			g_loger.StatusOut(L"点击重新观看按钮 X：%d Y：%d",ElemRect.rcElem.left+51,ElemRect.rcElem.top+382);
		}

	}
	
	if ( nIDEvent == TIME_ID_CHECK_VIDEO_PAUSE )
	{
		CElementInformation ElemInfo;
		CElemRectList ElemList;

		ElemInfo.SetTagName(L"object");
		ElemInfo.AddElementAttribute(L"id",L"VMSPlayer",TRUE);

		AutoBrowser.GetAllMatchElemRect(&ElemList,&ElemInfo);
		if ( ElemList.GetElemRectCount() == 1 )
		{
			// 51 382
			ELEM_RECT ElemRect;
			ElemList.GetElemRectByIndex(0,&ElemRect);
			
			CRect rcElem;
			rcElem = ElemRect.rcElem;
			CPoint ptCenter;
			ptCenter = rcElem.CenterPoint();

			COLORREF refColor = 0;

			HDC hDc = ::GetDC(GetIEServerWnd());
			refColor =  GetPixel(hDc,ptCenter.x,ptCenter.y);
			::ReleaseDC(GetIEServerWnd(),hDc);
			
			if ( m_refPreColor == refColor )
			{
				m_nColorSameCount++;
			}
			else
			{
				m_nColorSameCount=0;
			}

			m_refPreColor = refColor;

			if ( m_nColorSameCount > 20 )
			{
				g_loger.StatusOut(L"窗口定点颜色 %x 超过 20*5 秒没有变化",m_refPreColor);

				KillTimer(nIDEvent);

				CElementInformation ElemInfo;
				ElemInfo.SetTagName(L"input");
				ElemInfo.AddElementAttribute(L"value",L"退出学习",FALSE);
				//ElemInfo.AddElementAttribute(L"type",L"submit",TRUE);
				AutoBrowser.ClickFirstMatchWebPageElement(&ElemInfo);
			}
		
#ifdef DEBUG
			CString strMsgOut;
			strMsgOut.Format(L"0x%x\r\n",refColor);
			OutputDebugStringW(strMsgOut);
#endif
		}
	}

	CIECoreView::OnTimer(nIDEvent);
}

LRESULT CSXSView::OnVideoPause(WPARAM wParam,LPARAM lParam)
{
	SetTimer(TIME_ID_CLICK_RESUME,3000,NULL);

	g_loger.StatusOut(L"发现视屏被暂停，准备点击恢复按钮");

	return 0;
}

LRESULT CSXSView::OnVideoResume(WPARAM wParam,LPARAM lParam)
{
	//KillTimer(TIME_ID_CLICK_RESUME);

	//g_loger.StatusOut(L"视频重新观看，停止点击按钮");

	return 0;
}
