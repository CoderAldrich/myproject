
// ShuaClient.h : ShuaClient Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CShuaClientApp:
// �йش����ʵ�֣������ ShuaClient.cpp
//

class CShuaClientApp : public CWinApp
{
public:
	CShuaClientApp();

	CString m_strUserName;
	CString m_strPassWord;
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CShuaClientApp theApp;
