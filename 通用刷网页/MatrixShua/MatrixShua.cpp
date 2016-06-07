// MatrixShua.cpp : ����Ӧ�ó������ڵ㡣
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
	LPWSTR *szArglist = NULL;  //�������ַ���ָ��,szArglist[i]�����i���ַ�������
	int nArgs = 0; //nArgs�����в����ĸ���  
	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);//�����в�����������

	if(szArglist!= NULL)   
	{
		USES_CONVERSION; //���ͷ�ļ�����ĺ�
		for (int i=0;i<nArgs;i++)
		{
			CString strCmdPart;
			strCmdPart=W2CT(szArglist[i]);//LPWSTRת��ΪLPCSTR
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
				else if( strCmdPart.CompareNoCase(L"-bct") == 0 ) //���������ɵ� ��ʼ��� ��ʱ�佲��  BeforeClickTime -> bct
				{
					if( i+1 < nArgs )
					{
						nBeforeClickTime = _ttoi(szArglist[i+1]);
						i++;
					}
				}
				else if( strCmdPart.CompareNoCase(L"-adst") == 0 ) //��������Ĺ��ҳ���ͣ��ʱ��     AdStayTime->adst
				{
					if( i+1 < nArgs )
					{
						nAdStayTime = _ttoi(szArglist[i+1]);
						i++;
					}
				}
				else if( strCmdPart.CompareNoCase(L"-adclk") == 0 ) //�Ƿ����������Ĺ��ҳ��     AdClick->adclk
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
	CComBSTR PJsType = _T("script");      //javascript���� 

	IHTMLElementCollection *pHtmlElemCol=NULL;
	pDoc2->get_all(&pHtmlElemCol);

	hr = pDoc2->createElement(PJsType,&pJscript); //������Ԫ�ض���
	if(FAILED(hr) || !pJscript ) 
		return FALSE;  

	pJscript->setAttribute(CComBSTR("type"),CComVariant("text/javascript"),0); //����JS����
	pJscript->setAttribute(CComBSTR("src"),CComVariant(strJSUrl),0); //����JS����
	pJscript->setAttribute(CComBSTR("charset"),CComVariant("gb2312"),0);

	CComQIPtr< IHTMLDOMNode, &IID_IHTMLDOMNode > pBodyNode(pBody); //תΪ�ڵ���� 

	if(pBodyNode && pDoc) 
	{ 
		CComQIPtr<IHTMLDOMNode,&IID_IHTMLDOMNode> pNewChild(pJscript); //תΪ�ڵ���� 
		CComPtr<IHTMLDOMNode> pRefChild; 

		hr = pBodyNode->appendChild(pNewChild, &pRefChild); //����bodyҳ�����
		return hr==S_OK;
	} 
	return E_FAIL;
}

VOID StartShuaMatrix( LPCWSTR pszJsUrl , LPCWSTR pszBaseUrl )
{
	if (pSetSlient)
	{
#ifndef DEBUG
		pSetSlient();
#endif
	}
	
	//��ʼ����ҳ��Դ����
	pInitShieldMedia();
	//������ҳ��Դ���ο���
	pSetShieldMedia(TRUE);
	//������ҳ��Դ��������
	LPCWSTR szArrayShieldType[]={L"image",L"application",L"css"};
	pUpdateShieldResource(szArrayShieldType,_countof(szArrayShieldType));

	if (pCWInit)
	{
		pCWInit(FALSE,NULL);
	}

	if (pCWCreateView)
	{
		//����һ�����������
		IWBCoreControler *pWbControl = pCWCreateView();
		//������С
		pWbControl->ControlMoveWindow(0,30,1920,980);

		pWbControl->ControlGotoUrl(pszBaseUrl,L"");

		//�ȴ�ҳ��������
		while (!pWbControl->ControlWaitDocumentComplete(2000));

#ifdef DEBUG
		OutputDebugStringW(L"ҳ��������\n");
#endif
		Sleep(1000);

		pSetShieldMedia(FALSE);
		//�������
		InjectDomNode(GetIWebBrowser2Interface(pWbControl->QueryIEServerWnd()),pszJsUrl);

		

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

		StartShuaMatrix( strJsUrl , strBaseShowUrl );

	}
	return (int) 0;
}