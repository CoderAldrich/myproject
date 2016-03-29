#pragma once
#include <Shlwapi.h>
#include "..\LoadFileTimeout.h"
class CIconDownLoad
{
typedef struct ParamStruct
{
	HWND hWndNotify;
	UINT nMsg;
	WPARAM wParam;
	LPARAM lParam;
	CString IconUrl;
	CString SavePath;
} ParamStruct;
public:
	CIconDownLoad(void);
	~CIconDownLoad(void);

	HANDLE m_PreThreadHandle;
	static DWORD WINAPI DownLoadThread(LPVOID Param)
	{
		ParamStruct *param=(ParamStruct *)Param;
		
		if ( param == NULL )
		{
			return (DWORD)0;
		}

		CString IconUrl;
		CString cstrFilePath;

		IconUrl=param->IconUrl;
		cstrFilePath = param->SavePath;

		if( !PathFileExists(cstrFilePath) )//判断文件是否存在
		{
			DownloadFileTimeout(IconUrl,cstrFilePath,20000);
		}

		if ( param->hWndNotify!=NULL && IsWindow(param->hWndNotify))
		{
			::PostMessage(param->hWndNotify,param->nMsg,param->wParam,param->lParam);
		}

		delete param;
		
		return 0;
	}

	void StartDownload(HWND m_hMainFrame,UINT nMsg,WPARAM wParam,LPARAM lParam,CString IconUrl,CString SavePath)
	{
		ParamStruct *param=new ParamStruct; 
		param->hWndNotify=m_hMainFrame;
		param->nMsg = nMsg;
		param->wParam = wParam;
		param->lParam = lParam;
		param->IconUrl=IconUrl;
		param->SavePath = SavePath;

		m_PreThreadHandle=CreateThread(NULL,0,DownLoadThread,param,0,NULL);
	}
};
