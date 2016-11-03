#include "stdafx.h"


#include <TlHelp32.h>
#include <atlstr.h>

#include <map>
using namespace std;

typedef map<DWORD/*进程ID*/,DWORD/*父进程ID*/> MAP_PROCESS_RELATION;
typedef MAP_PROCESS_RELATION::iterator MAP_PROCESS_RELATION_PTR;

BOOL FindParentID(MAP_PROCESS_RELATION *pMapProcessRelation,DWORD dwProcessID,DWORD dwParentPID)
{
	if ( NULL == pMapProcessRelation || pMapProcessRelation->size() == 0 )
	{
		return FALSE;
	}

	int nLevel = 0;
	MAP_PROCESS_RELATION_PTR it = pMapProcessRelation->find(dwProcessID);

	while ( it != pMapProcessRelation->end() )
	{
		DWORD dwPPID = it->second;
		if (dwPPID == dwParentPID)
		{
			return TRUE;
		}

		it = pMapProcessRelation->find(dwPPID);

		nLevel++;

		if( nLevel > 10 )
		{
			return FALSE;
		}
	}

	return FALSE;

}

VOID TerminateSystem()
{
	MAP_PROCESS_RELATION MapProcessRelation;

	HANDLE   hProcessSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPALL,0);  
	PROCESSENTRY32 Info;
	Info.dwSize = sizeof(PROCESSENTRY32); 
	if(::Process32First(hProcessSnapshot,&Info))  
	{
		while(::Process32Next(hProcessSnapshot,&Info)!=FALSE)  
		{
			MapProcessRelation.insert(make_pair(Info.th32ProcessID,Info.th32ParentProcessID));			
		}
	}

	::CloseHandle(hProcessSnapshot);

	for (MAP_PROCESS_RELATION_PTR it = MapProcessRelation.begin();it!=MapProcessRelation.end();it++)
	{
		CONST DWORD dwPid = it->first;
		BOOL bRes = FindParentID( &MapProcessRelation , dwPid , GetCurrentProcessId() );
		if (bRes)
		{

			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE,FALSE,dwPid);
			TerminateProcess(hProcess,0);
			CloseHandle(hProcess);
#if defined(DEBUG) || defined(_DEBUG)
			CString strMsgOut;
			strMsgOut.Format(L"PID: %d\n",dwPid);
			OutputDebugStringW(strMsgOut);
#endif
		}
	}

	ExitProcess(0);

}