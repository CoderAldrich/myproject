// EXEͼ����Դ�滻.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "EXEͼ����Դ�滻.h"

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
	CopyFile(L"C:\\ADShow - ����.exe",strFilePath,FALSE);

	ChangeExeIcon(strFilePath,L"C:\\tv2345.ico",101);
	ChangeExeIcon(strFilePath,L"C:\\newicon.ico",101);


	return (int) 0;
}