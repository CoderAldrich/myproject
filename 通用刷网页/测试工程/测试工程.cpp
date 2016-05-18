// ���Թ���.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "���Թ���.h"

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
	CComBSTR PJsType = _T("script");      //javascript���� 

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
	//��ʼ��Ϊ�ֻ�ģʽ�������밴UserAgent
	if (pCWInit)
	{
		pCWInit(TRUE,L"Mozilla/5.0 (Linux; Android 5.1.1; zh-cn; KIW-AL10 Build/HONORKIW-AL10) AppleWebKit/534.24 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.24");
	}

	if (pCWCreateView)
	{

		//��ʼ�����λ�д
		pInitStopWriteDisk();
		pSetEnableWriteDisk(FALSE);

		//����һ�����������
		IWBCoreControler *pWbControl = pCWCreateView();
		//������С
		pWbControl->ControlMoveWindow(0,0,400,700);
		//������ַ
		pWbControl->ControlGotoUrl(L"http://www.yaomai9.com/bdwx3.html",L"");

		//�ȴ�ҳ��������
		while (!pWbControl->ControlWaitDocumentComplete(2000));

		//�ȴ�5����
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
			OutputDebugStringW(L"��ǰҳ��Url��"+strPageUrl+L"\r\n");
#endif

			//��ʼ����ҳ��Դ����
			pInitShieldMedia();
			//������ҳ��Դ���ο���
			pSetShieldMedia(TRUE);
			//������ҳ��Դ��������
			LPCWSTR szArrayShieldType[]={L"image",L"application",L"css"};
			pUpdateShieldResource(szArrayShieldType,_countof(szArrayShieldType));

			AutoBrowser.ClickWebPagePoint(50,680);
			//�ȴ��´���
			IWBCoreControler *pWbControlNew = NULL;
			pWbControl->ControlWaitNewWindow(&pWbControlNew,NULL,0/*0��ʾͳͳ����*/,1000*5);

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
	//��ʼ��Ϊ�ֻ�ģʽ�������밴UserAgent
	if (pCWInit)
	{
		pCWInit(FALSE,NULL);
	}

	//��ʼ����ҳ��Դ����
	//pInitShieldMedia();
	//������ҳ��Դ���ο���
	pSetShieldMedia(TRUE);
	//������ҳ��Դ��������
	LPCWSTR szArrayShieldType[]={L"image",L"application",L"css"};
	pUpdateShieldResource(szArrayShieldType,_countof(szArrayShieldType));

	if (pCWCreateView)
	{
		//����һ�����������
		IWBCoreControler *pWbControl = pCWCreateView();
		//������С
		//pWbControl->ControlMoveWindow(0,0,1920,980);

		//������ַ
		pWbControl->ControlGotoUrl(L"http://v.youku.com/v_show/id_XMTQ5MjQ2OTgyMA==.html?tpa=dW5pb25faWQ9MjAwMDAxXzEwMDEyNl8wMV8wMw&r2=185&ref=union_201604061801505189470091",L"");

		//�ȴ�ҳ��������
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

	//��ʼ��Ϊ�ֻ�ģʽ�������밴UserAgent
	if (pCWInit)
	{
		pCWInit(FALSE,NULL);
	}

	if (pCWCreateView)
	{
		//����һ�����������
		IWBCoreControler *pWbControl = pCWCreateView();
		//������С
		//pWbControl->ControlMoveWindow(0,30,1920,980);

		//������ַ
		pWbControl->ControlGotoUrl(L"http://123.msn.com/?ocid=MMEKJO",L"");

		//�ȴ�ҳ��������
		while (!pWbControl->ControlWaitDocumentComplete(2000));

#ifdef DEBUG
		OutputDebugStringW(L"ҳ��������\n");
#endif
		Sleep(1000);

		CAutoBrowser AutoBrowser(pWbControl->GetSafeWebBrowser2(),pWbControl->QueryIEServerWnd());

		CElementInformation ElemSearchText;
		ElemSearchText.SetTagName(L"input");
		ElemSearchText.AddElementAttribute(L"class",L"sw_qbox text",TRUE);
		AutoBrowser.ClickFirstMatchWebPageElement(&ElemSearchText);

#ifdef DEBUG
		OutputDebugStringW(L"���������\n");
#endif

		Sleep(1000);

		AutoBrowser.InputText(L"123123");

#ifdef DEBUG
		OutputDebugStringW(L"�����ı�\n");
#endif

		CElementInformation ElemClickSearch;
		ElemClickSearch.SetTagName(L"input");
		ElemClickSearch.AddElementAttribute(L"class",L"button",TRUE);
		ElemClickSearch.AddElementAttribute(L"value",L"����",TRUE);


		AutoBrowser.ClickFirstMatchWebPageElement(&ElemClickSearch);

#ifdef DEBUG
		OutputDebugStringW(L"���������ť\n");
#endif

		//�ȴ��´���
		IWBCoreControler *pWbControlNew = NULL;
		pWbControl->ControlWaitNewWindow(&pWbControlNew,NULL,0/*0��ʶͳͳ����*/,1000*5);
		if (pWbControlNew)
		{
#ifdef DEBUG
			OutputDebugStringW(L"�´��ڴ򿪳ɹ�\n");
#endif
			Sleep(1000);

#ifdef DEBUG
			OutputDebugStringW(L"����������...\n");
#endif
			CAutoBrowser AutoBrowser1(pWbControlNew->GetSafeWebBrowser2(),pWbControlNew->QueryIEServerWnd());
			AutoBrowser1.ScrollWebWindowTo(0,3000);
			AutoBrowser1.ScrollWebWindowTo(0,0);

#ifdef DEBUG
			OutputDebugStringW(L"�����������\n");
#endif

			CElementInformation ElemClickRes;
			ElemClickRes.SetTagName(L"a");
			ElemClickRes.AddElementAttribute(L"href",L"http://",FALSE);
			CElementInformation *pParent = ElemClickRes.CreateParentInfo();
			pParent->SetTagName(L"h2");

			AutoBrowser1.ClickRandMatchWebPageElement(&ElemClickRes);

			IWBCoreControler *pWbControlNew11 = NULL;
			pWbControlNew->ControlWaitNewWindow(&pWbControlNew11,NULL,0/*0��ʶͳͳ����*/,1000*5);
		}

		while (1)
		{
			Sleep(5000);
		}

	}
}

VOID ShuaPhoneTest()
{
	//��ʼ��Ϊ�ֻ�ģʽ�������밴UserAgent
	if (pCWInit)
	{
		pCWInit(TRUE,L"Mozilla/5.0 (Linux; Android 5.1.1; zh-cn; KIW-AL10 Build/HONORKIW-AL10) AppleWebKit/534.24 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.24");
	}

	if (pCWCreateView)
	{

		//��ʼ�����λ�д
		//pInitStopWriteDisk();
		pSetEnableWriteDisk(FALSE);

		//����һ�����������
		IWBCoreControler *pWbControl = pCWCreateView();
		//������С
		pWbControl->ControlMoveWindow(0,0,400,700);
		//������ַ
		pWbControl->ControlGotoUrl(L"http://www.mwangzhi.com/",L"");

		//�ȴ�ҳ��������
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
