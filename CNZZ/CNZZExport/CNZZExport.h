
// CNZZExport.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCNZZExportApp:
// �йش����ʵ�֣������ CNZZExport.cpp
//

class CCNZZExportApp : public CWinAppEx
{
public:
	CCNZZExportApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCNZZExportApp theApp;