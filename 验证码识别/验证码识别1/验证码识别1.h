
// ��֤��ʶ��1.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CVerCodeAnalysisApp:
// �йش����ʵ�֣������ ��֤��ʶ��1.cpp
//

class CVerCodeAnalysisApp : public CWinAppEx
{
public:
	CVerCodeAnalysisApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CVerCodeAnalysisApp theApp;