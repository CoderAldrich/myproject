
// RemoteXunLei.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRemoteXunLeiApp:
// �йش����ʵ�֣������ RemoteXunLei.cpp
//

class CRemoteXunLeiApp : public CWinAppEx
{
public:
	CRemoteXunLeiApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRemoteXunLeiApp theApp;