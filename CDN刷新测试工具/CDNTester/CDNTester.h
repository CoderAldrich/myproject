
// CDNTester.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCDNTesterApp:
// �йش����ʵ�֣������ CDNTester.cpp
//

class CCDNTesterApp : public CWinAppEx
{
public:
	CCDNTesterApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCDNTesterApp theApp;