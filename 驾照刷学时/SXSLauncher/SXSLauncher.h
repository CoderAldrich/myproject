
// SXSLauncher.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSXSLauncherApp:
// �йش����ʵ�֣������ SXSLauncher.cpp
//

class CSXSLauncherApp : public CWinAppEx
{
public:
	CSXSLauncherApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSXSLauncherApp theApp;