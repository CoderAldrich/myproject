// CommenWeb.h : CommenWeb DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCommenWebApp
// �йش���ʵ�ֵ���Ϣ������� CommenWeb.cpp
//

class CCommenWebApp : public CWinApp
{
public:
	CCommenWebApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
