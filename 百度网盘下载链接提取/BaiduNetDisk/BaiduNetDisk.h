
// ShuaClient.h : ShuaClient Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CShuaClientApp:
// �йش����ʵ�֣������ ShuaClient.cpp
//

class CBaiduNetDiskApp : public CWinApp
{
public:
	CBaiduNetDiskApp();
// ��д
public:
	virtual BOOL InitInstance();
// ʵ��
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CBaiduNetDiskApp theApp;
