
// ProcessMaster.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CProcessMasterApp:
// �йش����ʵ�֣������ ProcessMaster.cpp
//

class CProcessMasterApp : public CWinAppEx
{
public:
	CProcessMasterApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CProcessMasterApp theApp;