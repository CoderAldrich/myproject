
// SubWindowRunner.h : SubWindowRunner Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CSubWindowRunnerApp:
// �йش����ʵ�֣������ SubWindowRunner.cpp
//

class CSubWindowRunnerApp : public CWinApp
{
public:
	CSubWindowRunnerApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

public:
	DECLARE_MESSAGE_MAP()
};

extern CSubWindowRunnerApp theApp;
