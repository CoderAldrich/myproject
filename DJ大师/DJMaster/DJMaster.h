
// DJMaster.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDJMasterApp:
// �йش����ʵ�֣������ DJMaster.cpp
//

class CDJMasterApp : public CWinAppEx
{
public:
	CDJMasterApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDJMasterApp theApp;