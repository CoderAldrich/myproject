
// RemoteDebuggerController.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRemoteDebuggerControllerApp:
// �йش����ʵ�֣������ RemoteDebuggerController.cpp
//

class CRemoteDebuggerControllerApp : public CWinAppEx
{
public:
	CRemoteDebuggerControllerApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CRemoteDebuggerControllerApp theApp;