
// SuperYueChe.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSuperYueCheApp:
// �йش����ʵ�֣������ SuperYueChe.cpp
//

class CSuperYueCheApp : public CWinApp
{
public:
	CSuperYueCheApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSuperYueCheApp theApp;