// WinHookEx.h : WinHookEx DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CWinHookExApp
// �йش���ʵ�ֵ���Ϣ������� WinHookEx.cpp
//

class CWinHookExApp : public CWinApp
{
public:
	CWinHookExApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
