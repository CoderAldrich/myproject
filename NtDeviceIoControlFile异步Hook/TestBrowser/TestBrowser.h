
// TestBrowser.h : TestBrowser Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CTestBrowserApp:
// �йش����ʵ�֣������ TestBrowser.cpp
//

class CTestBrowserApp : public CWinApp
{
public:
	CTestBrowserApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CTestBrowserApp theApp;
