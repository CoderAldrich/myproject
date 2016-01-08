// Hao123View.cpp : 实现文件
//

#include "stdafx.h"
#include "PhoneView.h"
#include "AutoBrowser.h"
#include "MainFrm.h"


#include <WinInet.h>
#pragma comment(lib,"wininet.lib")


#ifdef _WIN64
#include <detours64.h>
#pragma comment(lib,"detours64.lib")
#else
#include <detours.h>
#pragma comment(lib,"detours.lib")
#endif


BOOL (WINAPI *pHttpSendRequestW)(
								 __in HINTERNET hRequest,
								 __in_ecount_opt(dwHeadersLength) LPCWSTR lpszHeaders,
								 __in DWORD dwHeadersLength,
								 __in_bcount_opt(dwOptionalLength) LPVOID lpOptional,
								 __in DWORD dwOptionalLength 
								 ) = HttpSendRequestW;
BOOL WINAPI MyHttpSendRequestW(
							   __in HINTERNET hRequest,
							   __in_ecount_opt(dwHeadersLength) LPCWSTR lpszHeaders,
							   __in DWORD dwHeadersLength,
							   __in_bcount_opt(dwOptionalLength) LPVOID lpOptional,
							   __in DWORD dwOptionalLength 
							   )
{
	
	CString strAddHeader;
	strAddHeader = L"User-Agent: "+CPhoneView::m_strUserAgent;
 	BOOL TReturn = pHttpSendRequestW(
 		hRequest,
 		strAddHeader.GetBuffer(),
 		strAddHeader.GetLength(),
 		lpOptional,
 		dwOptionalLength
 		);
	DWORD dwErrorCode = GetLastError();
	return TReturn;
};


typedef HRESULT (WINAPI *TypeGetPlatform)( IOmNavigator *pThis, BSTR *bstrPlatform );
TypeGetPlatform pGetNavigator = NULL;
HRESULT WINAPI MyGetPlatform( IOmNavigator *pThis, BSTR *bstrPlatform )
{
	CString strPlatform;
	strPlatform = CPhoneView::m_strPlatform;
	*bstrPlatform = strPlatform.AllocSysString();
#ifdef DEBUG
	OutputDebugStringW(L"伪装Platform信息："+strPlatform+L"\n");
#endif
	return S_OK;
}


typedef HRESULT (WINAPI *TypeGetUserAgent)( IOmNavigator *pThis, BSTR *bstrUserAgent );
TypeGetUserAgent pGetUserAgent = NULL;
HRESULT WINAPI MyGetUserAgent( IOmNavigator *pThis, BSTR *bstrUserAgent )
{
	CString strUserAgent;
	strUserAgent = CPhoneView::m_strUserAgent;
	*bstrUserAgent = strUserAgent.AllocSysString();
#ifdef DEBUG
	OutputDebugStringW(L"伪装UserAgent信息："+strUserAgent+L"\n");
#endif
	return S_OK;
}




IMPLEMENT_DYNCREATE(CPhoneView, CAutoBrowserView)

BOOL CPhoneView::bHook = FALSE;
BOOL CPhoneView::bInternalHook = FALSE;

CString CPhoneView::m_strUserAgent = L"Mozilla/5.0 (iPhone; CPU iPhone OS 8_0 like Mac OS X) AppleWebKit/600.1.3 (KHTML, like Gecko) Version/8.0 Mobile/12A4345d Safari/600.1.4\r\n";
CString CPhoneView::m_strPlatform = L"Linux armv7l";
CPhoneView::CPhoneView()
{

 	if( FALSE == bHook )
 	{
 		bHook = TRUE;
 
 		DetourTransactionBegin();
 		DetourUpdateThread(GetCurrentThread());
 
 		DetourAttach((LPVOID*)&pHttpSendRequestW,(PVOID)&MyHttpSendRequestW);
 
 		DetourTransactionCommit();
 	}
}

CPhoneView::~CPhoneView()
{
}

void CPhoneView::DoDataExchange(CDataExchange* pDX)
{
	CAutoBrowserView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPhoneView, CAutoBrowserView)
	ON_WM_TIMER()
END_MESSAGE_MAP()



#ifdef _DEBUG
void CPhoneView::AssertValid() const
{
	CAutoBrowserView::AssertValid();
}

void CPhoneView::Dump(CDumpContext& dc) const
{
	CAutoBrowserView::Dump(dc);
}
#endif //_DEBUG

VOID CPhoneView::SetUserAgentString(LPCWSTR pszUserAgent)
{
	m_strUserAgent = pszUserAgent;
}

VOID CPhoneView::SetPlatformString(LPCWSTR pszPlatform)
{
	m_strPlatform = pszPlatform;
}


void CPhoneView::OnInitialUpdate()
{
	CAutoBrowserView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
}
void CPhoneView::OnMainDocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
	__super::OnMainDocumentComplete(pDisp,URL);
	if ( FALSE == bInternalHook )
	{
		bInternalHook = TRUE;
		IHTMLDocument2 *pDoc2 = (IHTMLDocument2 *)GetHtmlDocument();
		if (pDoc2)
		{
			IHTMLWindow2 *pHW2 = NULL;
			pDoc2->get_parentWindow(&pHW2);



			IOmNavigator *pON = NULL;
			pHW2->get_navigator(&pON);

			DWORD dwTemp = *(DWORD *)((BYTE *)pON+0x10);
			pGetNavigator = (TypeGetPlatform)*((DWORD *)(dwTemp+0x58));

			pGetUserAgent =  (TypeGetUserAgent) *(PVOID *)((DWORD)(*(DWORD*)((BYTE *)pON+0xC+4))+0x28);

			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());

			DetourAttach((LPVOID*)&pGetNavigator,(PVOID)&MyGetPlatform);
			DetourAttach((LPVOID*)&pGetUserAgent,(PVOID)&MyGetUserAgent);

			DetourTransactionCommit();

		}
	}
}
