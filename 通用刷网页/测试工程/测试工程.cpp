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
typedef BOOL (WINAPI *TypeInitShieldMedia)();
typedef BOOL (WINAPI *TypeSetShieldMedia)(BOOL bSwitchOn);

TypeCWInit pCWInit = NULL;
TypeCWCreateView pCWCreateView = NULL;
TypeInitShieldMedia pInitShieldMedia = NULL;
TypeSetShieldMedia pSetShieldMedia = NULL;

VOID ShuaPhoneMatrix()
{
	//初始化为手机模式，并出入按UserAgent
	if (pCWInit)
	{
		pCWInit(TRUE,L"Mozilla/5.0 (Linux; Android 5.1.1; zh-cn; KIW-AL10 Build/HONORKIW-AL10) AppleWebKit/534.24 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.24");
	}

	if (pCWCreateView)
	{
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
			OutputDebugStringW(L"当前页面Url："+strPageUrl+L"\r\n");

			AutoBrowser.ClickWebPagePoint(50,680);

			//等待新窗口
			IWBCoreControler *pWbControlNew = NULL;
			pWbControl->ControlWaitNewWindow(&pWbControlNew,NULL,0/*0标识统统允许*/,1000*5);
			if (pWbControlNew)
			{
				pWbControlNew->ControlMoveWindow(0,0,400,700);
				WCHAR pszFileUrl[2000];
				pWbControlNew->ControlWaitDownloadFile(pszFileUrl,2000,INFINITE);
				break;
			}
		}

		int a=0;
	}
}

VOID ShuaYouku()
{
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
		pWbControl->ControlMoveWindow(0,0,1920,980);

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
		pInitShieldMedia = (TypeInitShieldMedia)GetProcAddress(hModule,"InitShieldMedia");
		pSetShieldMedia = (TypeSetShieldMedia)GetProcAddress(hModule,"SetShieldMedia");

		ShuaPhoneMatrix();
		//ShuaYouku();
	}
// 	while (1)
// 	{
// 		Sleep(1000);
// 	}
	return (int) 0;
}
