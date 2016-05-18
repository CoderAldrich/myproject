// 测试工程.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "测试工程.h"

#include "..\CommenWeb\IWBCoreControler.h"

#include "AutoBrowser.h"


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

typedef VOID (WINAPI *TypeCWInit)(BOOL bPhoneMode , LPCWSTR pszUserAgent);
typedef IWBCoreControler * (WINAPI *TypeCWCreateView)( );
typedef BOOL (WINAPI *TypeInitShieldResource)();
typedef BOOL (WINAPI *TypeUpdateShildType)( LPCWSTR *pszArrayTypes,int nTypesCount );
typedef BOOL (WINAPI *TypeSetShieldResource)(BOOL bSwitchOn);
typedef BOOL (WINAPI *TypeInitStopWriteDisk)();
typedef BOOL (WINAPI *TypeSetEnableWriteDisk)(BOOL bEnableWriteDisk);
typedef BOOL (WINAPI *TypeSetSlient)();

TypeCWInit pCWInit = NULL;
TypeCWCreateView pCWCreateView = NULL;
TypeInitShieldResource pInitShieldMedia = NULL;
TypeUpdateShildType pUpdateShieldResource = NULL;
TypeSetShieldResource pSetShieldMedia = NULL;
TypeInitStopWriteDisk pInitStopWriteDisk = NULL;
TypeSetEnableWriteDisk pSetEnableWriteDisk = NULL;
TypeSetSlient pSetSlient = NULL;

VOID ShuaPhoneMatrix()
{
	//初始化为手机模式，并出入按UserAgent
	if (pCWInit)
	{
		pCWInit(TRUE,L"Mozilla/5.0 (Linux; Android 5.1.1; zh-cn; KIW-AL10 Build/HONORKIW-AL10) AppleWebKit/534.24 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.24");
	}

	if (pCWCreateView)
	{

		//初始化屏蔽回写
		pInitStopWriteDisk();
		pSetEnableWriteDisk(FALSE);

		//创建一个浏览器窗口
		IWBCoreControler *pWbControl = pCWCreateView();
		//调整大小
		pWbControl->ControlMoveWindow(0,0,400,700);
		//导航网址
		pWbControl->ControlGotoUrl(L"http://www.yaomai9.com/bdwx3.html",L"");

		//等待页面加载完成
		while (!pWbControl->ControlWaitDocumentComplete(2000));

		//等待5秒钟
		Sleep(5000);
		//
		CAutoBrowser AutoBrowser(pWbControl->GetSafeWebBrowser2(),pWbControl->QueryIEServerWnd());

		for(int i=0;i<3;i++)
		{
			CString strPageUrl;
			pWbControl->ControlQueryUrl(strPageUrl.GetBuffer(2000),2000);
			strPageUrl.ReleaseBuffer();
			
			if (strPageUrl != L"http://www.yaomai9.com/bdwx3.html")
			{
				int a=0;
			}
#ifdef DEBUG
			OutputDebugStringW(L"当前页面Url："+strPageUrl+L"\r\n");
#endif

			//初始化网页资源屏蔽
			pInitShieldMedia();
			//设置网页资源屏蔽开关
			pSetShieldMedia(TRUE);
			//设置网页资源屏蔽类型
			LPCWSTR szArrayShieldType[]={L"image",L"application",L"css"};
			pUpdateShieldResource(szArrayShieldType,_countof(szArrayShieldType));

			AutoBrowser.ClickWebPagePoint(50,680);
			//等待新窗口
			IWBCoreControler *pWbControlNew = NULL;
			pWbControl->ControlWaitNewWindow(&pWbControlNew,NULL,0/*0表示统统允许*/,1000*5);

			if (pWbControlNew)
			{
				pWbControlNew->ControlMoveWindow(0,0,400,700);
				WCHAR pszFileUrl[2000];
				pWbControlNew->ControlWaitDownloadFile(pszFileUrl,2000,INFINITE);
				break;
			}
		}
	}
}

VOID ShuaYouku()
{
	//初始化为手机模式，并出入按UserAgent
	if (pCWInit)
	{
		pCWInit(FALSE,NULL);
	}

	//初始化网页资源屏蔽
	//pInitShieldMedia();
	//设置网页资源屏蔽开关
	pSetShieldMedia(TRUE);
	//设置网页资源屏蔽类型
	LPCWSTR szArrayShieldType[]={L"image",L"application",L"css"};
	pUpdateShieldResource(szArrayShieldType,_countof(szArrayShieldType));

	if (pCWCreateView)
	{
		//创建一个浏览器窗口
		IWBCoreControler *pWbControl = pCWCreateView();
		//调整大小
		//pWbControl->ControlMoveWindow(0,0,1920,980);

		//导航网址
		pWbControl->ControlGotoUrl(L"http://v.youku.com/v_show/id_XMTQ5MjQ2OTgyMA==.html?tpa=dW5pb25faWQ9MjAwMDAxXzEwMDEyNl8wMV8wMw&r2=185&ref=union_201604061801505189470091",L"");

		//等待页面加载完成
		while (!pWbControl->ControlWaitDocumentComplete(2000));

		while (1)
		{
			Sleep(5000);
		}

	}

}

VOID ShuaMsn()
{

	if (pSetSlient)
	{
#ifndef DEBUG
		pSetSlient();
#endif
	}

	//初始化为手机模式，并出入按UserAgent
	if (pCWInit)
	{
		pCWInit(FALSE,NULL);
	}

	if (pCWCreateView)
	{
		//创建一个浏览器窗口
		IWBCoreControler *pWbControl = pCWCreateView();
		//调整大小
		//pWbControl->ControlMoveWindow(0,30,1920,980);

		//导航网址
		pWbControl->ControlGotoUrl(L"http://123.msn.com/?ocid=MMEKJO",L"");

		//等待页面加载完成
		while (!pWbControl->ControlWaitDocumentComplete(2000));

#ifdef DEBUG
		OutputDebugStringW(L"页面加载完成\n");
#endif
		Sleep(1000);

		CAutoBrowser AutoBrowser(pWbControl->GetSafeWebBrowser2(),pWbControl->QueryIEServerWnd());

		CElementInformation ElemSearchText;
		ElemSearchText.SetTagName(L"input");
		ElemSearchText.AddElementAttribute(L"class",L"sw_qbox text",TRUE);
		AutoBrowser.ClickFirstMatchWebPageElement(&ElemSearchText);

#ifdef DEBUG
		OutputDebugStringW(L"点击搜索框\n");
#endif

		Sleep(1000);

		AutoBrowser.InputText(L"123123");

#ifdef DEBUG
		OutputDebugStringW(L"输入文本\n");
#endif

		CElementInformation ElemClickSearch;
		ElemClickSearch.SetTagName(L"input");
		ElemClickSearch.AddElementAttribute(L"class",L"button",TRUE);
		ElemClickSearch.AddElementAttribute(L"value",L"搜索",TRUE);


		AutoBrowser.ClickFirstMatchWebPageElement(&ElemClickSearch);

#ifdef DEBUG
		OutputDebugStringW(L"点击搜索按钮\n");
#endif

		//等待新窗口
		IWBCoreControler *pWbControlNew = NULL;
		pWbControl->ControlWaitNewWindow(&pWbControlNew,NULL,0/*0标识统统允许*/,1000*5);
		if (pWbControlNew)
		{
#ifdef DEBUG
			OutputDebugStringW(L"新窗口打开成功\n");
#endif
			Sleep(1000);

#ifdef DEBUG
			OutputDebugStringW(L"滚动窗口中...\n");
#endif
			CAutoBrowser AutoBrowser1(pWbControlNew->GetSafeWebBrowser2(),pWbControlNew->QueryIEServerWnd());
			AutoBrowser1.ScrollWebWindowTo(0,3000);
			AutoBrowser1.ScrollWebWindowTo(0,0);

#ifdef DEBUG
			OutputDebugStringW(L"滚动窗口完成\n");
#endif

			CElementInformation ElemClickRes;
			ElemClickRes.SetTagName(L"a");
			ElemClickRes.AddElementAttribute(L"href",L"http://",FALSE);
			CElementInformation *pParent = ElemClickRes.CreateParentInfo();
			pParent->SetTagName(L"h2");

			AutoBrowser1.ClickRandMatchWebPageElement(&ElemClickRes);

			IWBCoreControler *pWbControlNew11 = NULL;
			pWbControlNew->ControlWaitNewWindow(&pWbControlNew11,NULL,0/*0标识统统允许*/,1000*5);
		}

		while (1)
		{
			Sleep(5000);
		}

	}
}

VOID ShuaPhoneTest()
{
	//初始化为手机模式，并出入按UserAgent
	if (pCWInit)
	{
		pCWInit(TRUE,L"Mozilla/5.0 (Linux; Android 5.1.1; zh-cn; KIW-AL10 Build/HONORKIW-AL10) AppleWebKit/534.24 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.24");
	}

	if (pCWCreateView)
	{

		//初始化屏蔽回写
		//pInitStopWriteDisk();
		pSetEnableWriteDisk(FALSE);

		//创建一个浏览器窗口
		IWBCoreControler *pWbControl = pCWCreateView();
		//调整大小
		pWbControl->ControlMoveWindow(0,0,400,700);
		//导航网址
		pWbControl->ControlGotoUrl(L"http://www.mwangzhi.com/",L"");

		//等待页面加载完成
		while (!pWbControl->ControlWaitDocumentComplete(2000));

		

// 		while (1)
// 		{
// 			WCHAR szFileUrl[1000]={0};
// 			pWbControl->ControlWaitDownloadFile(szFileUrl,1000,2000);
// 			if (wcslen(szFileUrl))
// 			{
// 				break;
// 			}
// 		}


		IWBCoreControler *pNewWbControl = NULL;

		while (1)
		{
			pWbControl->ControlWaitNewWindow(&pNewWbControl,NULL,0,2000);
			if (pNewWbControl)
			{
				break;
			}
		}

		pNewWbControl->ControlMoveWindow(100,100,400,700);

		Sleep(1000000);

	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	HMODULE hModule = ::LoadLibraryW(L".\\CommenWeb.dll");
	if (hModule)
	{
		pCWInit = (TypeCWInit)GetProcAddress(hModule,"CWInit");
		pCWCreateView = (TypeCWCreateView)GetProcAddress(hModule,"CWCreateView");
		pInitShieldMedia = (TypeInitShieldResource)GetProcAddress(hModule,"InitShieldResource");
		pUpdateShieldResource = (TypeUpdateShildType)GetProcAddress(hModule,"UpdateShieldType");
		pSetShieldMedia = (TypeSetShieldResource)GetProcAddress(hModule,"SetShieldResource");
		pInitStopWriteDisk = (TypeInitStopWriteDisk)GetProcAddress(hModule,"InitStopWriteDisk");
		pSetEnableWriteDisk = (TypeSetEnableWriteDisk)GetProcAddress(hModule,"SetEnableWriteDisk");
		pSetSlient = (TypeSetSlient)GetProcAddress(hModule,"SetSlient");

#ifndef DEBUG
		pSetSlient();
#endif
		//ShuaPhoneMatrix();
		//ShuaYouku();
		//ShuaMsn();
		ShuaPhoneTest();
	}
// 	while (1)
// 	{
// 		Sleep(1000);
// 	}
	return (int) 0;
}
