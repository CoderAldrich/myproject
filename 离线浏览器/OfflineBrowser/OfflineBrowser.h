
// OfflineBrowser.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// COfflineBrowserApp:
// �йش����ʵ�֣������ OfflineBrowser.cpp
//

class COfflineBrowserApp : public CWinAppEx
{
public:
	COfflineBrowserApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern COfflineBrowserApp theApp;