// CommenWeb.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "CommenWeb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CCommenWebApp

BEGIN_MESSAGE_MAP(CCommenWebApp, CWinApp)
END_MESSAGE_MAP()


// CCommenWebApp ����

CCommenWebApp::CCommenWebApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CCommenWebApp ����

CCommenWebApp theApp;


// CCommenWebApp ��ʼ��

BOOL CCommenWebApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
