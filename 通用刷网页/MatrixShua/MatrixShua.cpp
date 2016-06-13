// MatrixShua.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"

#include "MatrixShua.h"
#include <ShellAPI.h>
#include <OleAcc.h>
#pragma comment(lib,"oleacc.lib")
#include <ServProv.h>
#include <ShlGuid.h>



#include "..\CommenWeb\CommenWebExport.h"

#include "AutoBrowser.h"
#include "Base64Decode.h"
#include "RandHome.h"

TypeCWInit pCWInit = NULL;
TypeCWCreateView pCWCreateView = NULL;
TypeInitShieldResource pInitShieldMedia = NULL;
TypeUpdateShildType pUpdateShieldResource = NULL;
TypeSetShieldResource pSetShieldMedia = NULL;
TypeInitStopWriteDisk pInitStopWriteDisk = NULL;
TypeSetEnableWriteDisk pSetEnableWriteDisk = NULL;
TypeSetSlient pSetSlient = NULL;

VOID MyParseCommandLine( 
						CString &strJsUrl,
						BOOL &bClickMatrix,
						int &nClickMatrixType,
						BOOL &bPhoneBrowser,
						int &nBeforeClickTime,
						int &nAdStayTime,
						BOOL &bAdClick,
						CString &strBaseUrl,
						BOOL &bHaveMatrix,
						UINT  &nMatrixPos
						)
{
	LPWSTR *szArglist = NULL;  //命令行字符串指针,szArglist[i]代表第i个字符串变量
	int nArgs = 0; //nArgs命令行参数的个数  
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);//命令行参数解析函数

	if(szArglist!= NULL)   
	{
		USES_CONVERSION; //相关头文件定义的宏
		for (int i=0;i<nArgs;i++)
		{
			CString strCmdPart;
			strCmdPart=W2CT(szArglist[i]);//LPWSTR转换为LPCSTR
#ifdef DEBUG
			OutputDebugStringW(strCmdPart+L"\n");
#endif
			if (strCmdPart.GetAt(0) == L'-')
			{
				if(strCmdPart.CompareNoCase(L"-cm") == 0)
				{
					if( i+1 < nArgs )
					{
						bClickMatrix = _ttoi(szArglist[i+1]);
						i++;
					}

				}
				else if(strCmdPart.CompareNoCase(L"-cmt") == 0)
				{
					if( i+1 < nArgs )
					{
						nClickMatrixType = _ttoi(szArglist[i+1]);
						i++;
					}
				}
				else if(strCmdPart.CompareNoCase(L"-u") == 0)
				{
					if( i+1 < nArgs )
					{
						strJsUrl = szArglist[i+1];

						CStringA straUrl;
						straUrl = strJsUrl;
						int nRetLen = 0;
						string strDecodeUrl = Base64_Decode(straUrl.GetBuffer(),straUrl.GetLength(),nRetLen);

						strJsUrl = strDecodeUrl.c_str();

						i++;
					}
				}
				else if(strCmdPart.CompareNoCase(L"-t") == 0)
				{
					if( i+1 < nArgs )
					{
						CString strBrowserType;

						strBrowserType = szArglist[i+1];

						bPhoneBrowser = (strBrowserType.CompareNoCase(L"m") == 0);

						i++;
					}
				}
				else if( strCmdPart.CompareNoCase(L"-bct") == 0 ) //插入矩阵完成到 开始点击 的时间讲个  BeforeClickTime -> bct
				{
					if( i+1 < nArgs )
					{
						nBeforeClickTime = _ttoi(szArglist[i+1]);
						i++;
					}
				}
				else if( strCmdPart.CompareNoCase(L"-adst") == 0 ) //点击出来的广告页面的停留时间     AdStayTime->adst
				{
					if( i+1 < nArgs )
					{
						nAdStayTime = _ttoi(szArglist[i+1]);
						i++;
					}
				}
				else if( strCmdPart.CompareNoCase(L"-adclk") == 0 ) //是否点击弹出来的广告页面     AdClick->adclk
				{
					if( i+1 < nArgs )
					{
						bAdClick = _ttoi(szArglist[i+1]);
						i++;
					}
				}
				else if( strCmdPart.CompareNoCase(L"-burl") == 0 )
				{
					if( i+1 < nArgs )
					{
						strBaseUrl = szArglist[i+1];

						CStringA straBaseUrl;
						straBaseUrl = strBaseUrl;
						int nRetLen = 0;
						string strDecodeUrl = Base64_Decode(straBaseUrl.GetBuffer(),straBaseUrl.GetLength(),nRetLen);

						strBaseUrl = strDecodeUrl.c_str();

						i++;
					}
				}
				else if( strCmdPart.CompareNoCase(L"-hmt") == 0 )
				{
					if( i+1 < nArgs )
					{
						bHaveMatrix = _ttoi(szArglist[i+1]);
						i++;
					}
				}
				else if( strCmdPart.CompareNoCase(L"-mpos") == 0 )
				{
					if( i+1 < nArgs )
					{
						nMatrixPos = _ttoi(szArglist[i+1]);
						i++;
					}
				}

			}
		}
		LocalFree(szArglist);  
	} 

}



BOOL InjectDomNode(IWebBrowser *pWb,CString strJSUrl)
{
	if (pWb == NULL)
	{
		return FALSE;
	}

	CComQIPtr<IDispatch>    pDisp;
	CComQIPtr<IHTMLDocument> pDoc;
	HRESULT hr = pWb->get_Document((IDispatch **)&pDisp);

	if (pDisp)
	{
		hr = pDisp->QueryInterface(IID_IHTMLDocument2,(void **)&pDoc);
	}

	if ( FAILED(hr) || pDoc == NULL )
	{
		return FALSE;
	}

	CComQIPtr<IHTMLDocument2> pDoc2(pDoc);
	if (pDoc2 == NULL)
	{
		return FALSE;
	}

	CComQIPtr<IHTMLBodyElement> pBody;
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

	IHTMLElementCollection *pHtmlElemCol=NULL;
	pDoc2->get_all(&pHtmlElemCol);

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

IWebBrowser2 * GetIWebBrowser2Interface(HWND BrowserWnd) 
{
	CoInitialize(NULL);

	HRESULT hr;
	LRESULT lRes; 
	const UINT nMsg = ::RegisterWindowMessage( L"WM_HTML_GETOBJECT" );
	::SendMessageTimeout( BrowserWnd, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 1000, (DWORD_PTR*)&lRes );
	static LPFNOBJECTFROMLRESULT pfObjectFromLresult = NULL;
	if ( NULL == pfObjectFromLresult )
	{
		HINSTANCE hInst = ::LoadLibrary( L"OLEACC.DLL" );
		if ( hInst )
		{
			pfObjectFromLresult = (LPFNOBJECTFROMLRESULT)::GetProcAddress( hInst, "ObjectFromLresult" );
		}
	}
	if ( pfObjectFromLresult  )
	{
		CComPtr<IServiceProvider> spServiceProv;
		hr = (*pfObjectFromLresult)( lRes, IID_IServiceProvider, 0, (void**)&spServiceProv );
		if ( SUCCEEDED(hr) )
		{
			IWebBrowser2* pWebBrowser2=NULL;
			hr = spServiceProv->QueryService(SID_SWebBrowserApp,
				IID_IWebBrowser2,(void**)&pWebBrowser2);
			return pWebBrowser2;
		} 
	}

	CoUninitialize();

	return NULL;
}

VOID StartShuaMatrix( LPCWSTR pszJsUrl , LPCWSTR pszBaseUrl , BOOL bClickMatrix , DWORD dwBeforeClickTime , DWORD dwAdStayTime )
{
	if (pSetSlient)
	{
#ifndef DEBUG
		pSetSlient();
#endif
	}
	
	//初始化网页资源屏蔽
	pInitShieldMedia();
	//设置网页资源屏蔽开关
	pSetShieldMedia(TRUE);
	//设置网页资源屏蔽类型
	LPCWSTR szArrayShieldType[]={L"image",L"application",L"css"};
	pUpdateShieldResource(szArrayShieldType,_countof(szArrayShieldType));

	if (pCWInit)
	{
		pCWInit(FALSE,NULL);
	}

	if (pCWCreateView)
	{
		CRect rtWorkArea;
		SystemParametersInfo(SPI_GETWORKAREA,0,&rtWorkArea,0) ;   // 获得工作区大小


		//创建一个浏览器窗口
		IWBCoreControler *pWbControl = pCWCreateView();
		//调整大小
		pWbControl->ControlMoveWindow(0,0,rtWorkArea.Width(),rtWorkArea.Height());

		pWbControl->ControlGotoUrl(pszBaseUrl,L"");

		//等待页面加载完成
		for(int i=0;i<5;i++)
		{
			if(pWbControl->ControlWaitDocumentComplete(2000))
			{
				break;
			}
		}

		//pWbControl->ControlStopLoading();

#ifdef DEBUG
		OutputDebugStringW(L"页面加载完成\n");
#endif
		Sleep(1000);

		pSetShieldMedia(FALSE);
		//插入矩阵
		InjectDomNode(pWbControl->GetSafeWebBrowser(),pszJsUrl);

<<<<<<< .mine
		//模拟鼠标移动
		CAutoBrowser AutoBrowser(pWbControl->GetSafeWebBrowser2(),pWbControl->QueryIEServerWnd());

||||||| .r131
		

=======
		CAutoBrowser AutoBrowser(pWbControl->GetSafeWebBrowser2(),pWbControl->QueryIEServerWnd());
		AutoBrowser.ScrollWebWindowTo(0,300);
>>>>>>> .r132
		for ( int i=0;i<dwBeforeClickTime;i++)
		{
			CRect rcIEServerWnd;
			HWND hIEServerWnd = pWbControl->QueryIEServerWnd();
			::GetClientRect(hIEServerWnd,&rcIEServerWnd);

			AutoBrowser.SetWebPageMousePos(CAutoBrowser::GetRandValue(0,rcIEServerWnd.Width()),CAutoBrowser::GetRandValue(0,rcIEServerWnd.Height()));
			Sleep(1000);
		}
		
		//如果点击矩阵
		if ( bClickMatrix )
		{
			IWBCoreControler *pNewWbControl = NULL;

			CRect rcMatrix;
			HWND hIEServerWnd = pWbControl->QueryIEServerWnd();
			::GetClientRect(hIEServerWnd,&rcMatrix);
			rcMatrix.left = rcMatrix.right-300;
			rcMatrix.top = rcMatrix.bottom-250;
			
			rcMatrix.left+=30;
			rcMatrix.right-=60;

			//最多尝试五次点击
			for(int i=0;i<5;i++)
			{
				CPoint ptClick = CAutoBrowser::GetRandPointInRect(rcMatrix);
				AutoBrowser.ClickWebPagePoint(ptClick.x,ptClick.y);	
				pWbControl->ControlWaitNewWindow(&pNewWbControl,NULL,0,2000);
				if (pNewWbControl)
				{
					break;
				}
			}

			if ( pNewWbControl  )
			{
				CRect rcIEServerWnd;
				HWND hIEServerWnd = pWbControl->QueryIEServerWnd();
				::GetClientRect(hIEServerWnd,&rcIEServerWnd);

				CAutoBrowser AutoBrowserNew(pNewWbControl->GetSafeWebBrowser2(),pNewWbControl->QueryIEServerWnd());
				for (int i=0;i<dwAdStayTime;i++)
				{
					AutoBrowserNew.SetWebPageMousePos(CAutoBrowser::GetRandValue(0,rcIEServerWnd.Width()),CAutoBrowser::GetRandValue(0,rcIEServerWnd.Height()));
					Sleep(1000);
				}
				
			}

			//pSetShieldMedia(TRUE);

			
		}

// 		while (1)
// 		{
// 			Sleep(5000);
// 		}

	}
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	

	CString strJsUrl;
	BOOL bClickMatrix = FALSE;
	int nClickMatrixType = 0;
	BOOL bPhoneBrowser = FALSE;
	int nBeforeClickTime = 0;
	int nAdStayTime = 0;
	BOOL bAdClick = FALSE;
	CString strBaseUrl;
	BOOL    bHaveMatrix = FALSE;
	UINT    nMatrixPos = 3;
	MyParseCommandLine( 
		strJsUrl,
		bClickMatrix,
		nClickMatrixType,
		bPhoneBrowser,
		nBeforeClickTime,
		nAdStayTime,
		bAdClick,
		strBaseUrl,
		bHaveMatrix,
		nMatrixPos
		);

	CString strBaseShowUrl;
	CString strReferUrl;

	if ( strBaseUrl.GetLength() > 0 )
	{
		strBaseShowUrl = strBaseUrl;
		strReferUrl = L"";
	}
	else
	{
		BOOL bGetHotUrl = FALSE;
		int nHotRand = CAutoBrowser::GetRandValue(1,100);
		bGetHotUrl = (nHotRand <= 20);

		if (bGetHotUrl)
		{
			strBaseShowUrl = GetHotUrl();
		}
		else
		{
			strBaseShowUrl = GetRandUrl(strReferUrl);
		}

		for (int i=0;i<1 && (strBaseShowUrl.GetLength() == 0);i++)
		{
			if (bGetHotUrl)
			{
				strBaseShowUrl = GetHotUrl();
			}
			else
			{
				strBaseShowUrl = GetRandUrl(strReferUrl);
			}
		}
	}


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


		StartShuaMatrix( strJsUrl , strBaseShowUrl ,bClickMatrix,nBeforeClickTime, nAdStayTime );

	}
	return (int) 0;
}