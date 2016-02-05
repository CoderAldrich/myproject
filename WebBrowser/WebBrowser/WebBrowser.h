
// WebBrowser.h : WebBrowser 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
//#include "WebBrowser_i.h"

//#include "disputil.h"
// CWebBrowserApp:
// 有关此类的实现，请参阅 WebBrowser.cpp
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
// 重写
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
