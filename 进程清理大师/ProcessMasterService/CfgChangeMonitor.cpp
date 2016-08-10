#include "stdafx.h"
#include <Windows.h>
#include <atlstr.h>

typedef struct _tagFILE_MONITOR_PARAM
{
	WCHAR szFilePath[MAX_PATH];
	HANDLE hEvent;
}FILE_MONITOR_PARAM,*PFILE_MONITOR_PARAM;

DWORD WINAPI FileModifyMonitorThread( LPVOID lpParam )  
{  

	PFILE_MONITOR_PARAM pFileParam = (PFILE_MONITOR_PARAM)lpParam;

	if ( NULL == pFileParam || 0 == wcslen(pFileParam->szFilePath) || pFileParam->hEvent == NULL )
	{
		return 0;
	}

	CString strFilePath;
	CString strFileName;
	strFileName = strFilePath = pFileParam->szFilePath;

	strFilePath = strFilePath.Left(strFilePath.ReverseFind(L'\\')+1);
	strFileName = strFileName.Right(strFileName.GetLength() - strFileName.ReverseFind(L'\\')-1);

	HANDLE hRootDirChangeHandle = CreateFileW(  
		strFilePath, 
		FILE_LIST_DIRECTORY,                
		FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, 
		NULL, 
		OPEN_EXISTING, 
		FILE_FLAG_BACKUP_SEMANTICS, 
		NULL 
		); 
	if (hRootDirChangeHandle == INVALID_HANDLE_VALUE) 
	{ 
		return 0; 
	} 

	char notify[1024]; 
	memset(notify, 0, 1024); 
	DWORD cbBytes;  
	FILE_NOTIFY_INFORMATION *pNotify=(FILE_NOTIFY_INFORMATION *)notify; 

	while ( true ) 
	{    
		if(ReadDirectoryChangesW(hRootDirChangeHandle, &notify, sizeof(notify), 
			FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE , &cbBytes, NULL, NULL)) 
		{            
			switch(pNotify->Action) 
			{ 
			case FILE_ACTION_MODIFIED:
				{
					if ( strFileName.CompareNoCase( pNotify->FileName ) == 0 )
					{
						static DWORD dwLastNotify = 0;
						if ( GetTickCount() - dwLastNotify > 100 )
						{
							dwLastNotify = GetTickCount();
							SetEvent(pFileParam->hEvent);
						}
					}

				}
				break; 
			default: 
				{

				}
			}        

		} 
		
		memset(notify, 0, 1024); 
	} 
	::CloseHandle(hRootDirChangeHandle); 


	return 0;  
} 


VOID FileModifyMonitor( LPCWSTR pszFilePath,HANDLE hEvent )
{
	static BOOL bRunning = FALSE;
	if ( FALSE == bRunning )
	{
		bRunning = TRUE;

		FILE_MONITOR_PARAM *pParam = new FILE_MONITOR_PARAM;
		wcscpy_s(pParam->szFilePath,MAX_PATH,pszFilePath);
		pParam->hEvent = hEvent;

		CreateThread(NULL,0,FileModifyMonitorThread,pParam,0,NULL);
	}
}
