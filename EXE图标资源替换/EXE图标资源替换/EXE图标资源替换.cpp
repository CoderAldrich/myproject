// EXE图标资源替换.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "EXE图标资源替换.h"

#include <atlstr.h>

#include "ExeIconChange.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	CString strFilePath;
	strFilePath.Format(L"C:\\ADShow%d.exe",GetTickCount());

	DeleteFile(strFilePath);
	CopyFile(L"C:\\ADShow - 副本.exe",strFilePath,FALSE);

	ChangeExeIcon(strFilePath,L"C:\\tv2345.ico",101);
	ChangeExeIcon(strFilePath,L"C:\\newicon.ico",101);


	return (int) 0;
}