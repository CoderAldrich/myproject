
// WebBrowser.h : WebBrowser Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
//#include "WebBrowser_i.h"

//#include "disputil.h"
// CWebBrowserApp:
// �йش����ʵ�֣������ WebBrowser.cpp
//

class CWebBrowserApp : public CWinApp
{
public:
	CWebBrowserApp();
private:
	LONG m_dwFrameCount;
	CComPtr<IGlobalInterfaceTable> spGIT;
public:
	
	CString strHomePage;
	CString strSearchID;
	UINT m_nBaseStyle;
	BOOL m_bMutiTab;
	BOOL m_bMenuBar;
	BOOL m_bToolBar;
	BOOL m_bCommandBar;

private:
// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
public:

	int Run();

	IGlobalInterfaceTable *GetGITPtr();
	void AddFrameThread(HANDLE hFrameThread);
	void DelFrameThread(HANDLE hFrameThread);

	void IncreaseFrameCount(void);
	void DecreaseFrameCount(void);
	BOOL ExitInstance(void);
};

extern CWebBrowserApp theApp;
