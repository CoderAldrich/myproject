
// DllInjecter.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDllInjecterApp:
// �йش����ʵ�֣������ DllInjecter.cpp
//

class CDllInjecterApp : public CWinApp
{
public:
	CDllInjecterApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDllInjecterApp theApp;