// CommenWeb.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "CommenWeb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CCommenWebApp

BEGIN_MESSAGE_MAP(CCommenWebApp, CWinApp)
END_MESSAGE_MAP()


// CCommenWebApp 构造

CCommenWebApp::CCommenWebApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CCommenWebApp 对象

CCommenWebApp theApp;


// CCommenWebApp 初始化

BOOL CCommenWebApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
