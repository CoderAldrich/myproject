
// ProcCleaner.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CProcCleanerApp:
// �йش����ʵ�֣������ ProcCleaner.cpp
//

class CProcCleanerApp : public CWinAppEx
{
public:
	CProcCleanerApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CProcCleanerApp theApp;