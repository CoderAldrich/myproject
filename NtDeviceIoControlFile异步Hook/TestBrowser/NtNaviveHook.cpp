#include "stdafx.h"
#include "NtNaviveHook.h"


#define AFD_RECV 0x12017
#define AFD_SEND 0x1201f
#define STATUS_UNSUCCESSFUL              ((NTSTATUS)0xC0000001L)
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L) // ntsubauth
#define STATUS_DEVICE_NOT_READY          ((NTSTATUS)0xC00000A3L)

LPCSTR GetInjectString();
BOOL CheckInjectPass(LPCSTR pszUrl);

#if defined(DEBUG) || defined(_DEBUG)

VOID DebugStringW(const WCHAR* fmt, ...)
{
	return ;
	WCHAR mtBuff[MAX_PATH]={0};

	va_list argptr;
	va_start(argptr, fmt);//_vsntprintf
	int cbBuf = _vsntprintf_s(mtBuff, MAX_PATH,_TRUNCATE, fmt, argptr) + 1;
	WCHAR *lpBuf = new WCHAR[cbBuf+10];
	_vsnwprintf_s(lpBuf, cbBuf, _TRUNCATE,fmt, argptr);
	wcscat_s(lpBuf,cbBuf+10,L"\n");
	OutputDebugStringW(lpBuf);

	delete lpBuf;
}


VOID DebugStringA(const CHAR* fmt, ...)
{
	//return ;
	CHAR mtBuff[MAX_PATH]={0};

	va_list argptr;
	va_start(argptr, fmt);//_vsntprintf
	int cbBuf = _vsnprintf_s(mtBuff, MAX_PATH,_TRUNCATE, fmt, argptr) + 1;
	CHAR *lpBuf = new CHAR[cbBuf+10];
	_vsnprintf_s(lpBuf, cbBuf, _TRUNCATE,fmt, argptr);
	strcat_s(lpBuf,cbBuf+10,"\n");
	OutputDebugStringA(lpBuf);

	delete lpBuf;
}
#else

#define DebugStringW(...) 
#define DebugStringA(...) 

#endif
typedef struct AFD_WSABUF{
	UINT  len ;
	PCHAR  buf ;
}AFD_WSABUF , *PAFD_WSABUF;

typedef struct AFD_INFO {
	PAFD_WSABUF  BufferArray ; 
	ULONG        BufferCount ; 
	ULONG        AfdFlags ;
	ULONG        TdiFlags ;
} AFD_INFO,  *PAFD_INFO;

typedef LONG NTSTATUS;

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

TypeNtDeviceIoControlFile pNtDeviceIoControlFile = NULL;

DWORD dwTlsIndex = 0; 


class CCSLock
{
private:
	CRITICAL_SECTION m_cs;
public:
	CCSLock()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CCSLock()
	{
		DeleteCriticalSection(&m_cs);
	}
	VOID Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	VOID UnLock()
	{
		LeaveCriticalSection(&m_cs);
	}

};


#include <list>
#include <hash_map>
using namespace std;
using namespace stdext;

typedef struct tagNOTIFY_PARAM
{
	HANDLE hMyEvent;
	HANDLE hFile;
	HANDLE hEvent;
	char * pRecvBuf;
	ULONG nRecvBufLen;
	IO_STATUS_BLOCK *pIoStatus;
}NOTIFY_PARAM,*PNOTIFY_PARAM;

typedef list<PNOTIFY_PARAM> LIST_TASK,*PQUEUE_TASK;
typedef LIST_TASK::iterator LIST_TASK_PTR;

typedef struct tagQUEUE_WAIT_TASK
{
	HANDLE hEventTask;
	CCSLock locker;
	LIST_TASK QueueTask;
}QUEUE_WAIT_TASK,*PQUEUE_WAIT_TASK;

typedef hash_map<HANDLE,char> HANDLE_HASH_MAP,*PHANDLE_HASH_MAP;
typedef HANDLE_HASH_MAP::iterator HANDLE_HASH_MAP_PTR;

CCSLock         HandleMapLock;
HANDLE_HASH_MAP HandleMap;

VOID RecordHandle( HANDLE hFile )
{
	HandleMapLock.Lock();

	HANDLE_HASH_MAP_PTR it = HandleMap.find(hFile);
	if (it == HandleMap.end())
	{
		HandleMap[hFile] = 1;
	}

	HandleMapLock.UnLock();
}

BOOL CheckHandle(HANDLE hFile)
{
	BOOL bRes = FALSE;

	HandleMapLock.Lock();

	HANDLE_HASH_MAP_PTR it = HandleMap.find(hFile);
	if (it != HandleMap.end())
	{
		bRes = TRUE;
	}

	HandleMapLock.UnLock();

	return bRes;
}

VOID DelHandle(HANDLE hFile)
{
	HandleMapLock.Lock();

	HANDLE_HASH_MAP_PTR it = HandleMap.find(hFile);
	if (it != HandleMap.end())
	{
		HandleMap.erase(it);
	}

	HandleMapLock.UnLock();
}

DWORD WINAPI WaitThread(PVOID pParam)
{
	PQUEUE_WAIT_TASK pQueueTask = (PQUEUE_WAIT_TASK)pParam;

	while (TRUE)
	{
		WaitForSingleObject(pQueueTask->hEventTask,500);
		LIST_TASK TmpTask;
		pQueueTask->locker.Lock();

		for(LIST_TASK_PTR it = pQueueTask->QueueTask.begin();it!=pQueueTask->QueueTask.end();it++)
		{
			TmpTask.push_back(*it);
		}

		CString strMsgOut;
		strMsgOut.Format(L"******TaskSize: %d \r\n",pQueueTask->QueueTask.size());
		OutputDebugStringW(strMsgOut);

		pQueueTask->QueueTask.clear();

		pQueueTask->locker.UnLock();

		
		while ( TmpTask.size() > 0 )
		{
	
			LIST_TASK TmpTmpTask;

			for(LIST_TASK_PTR it = TmpTask.begin();it!=TmpTask.end();it++)
			{
				BOOL bCanRemove = TRUE;

				if ( CheckHandle((*it)->hFile) )
				{
					bCanRemove = FALSE;

					DWORD dwWaitRes = WaitForSingleObject((*it)->hMyEvent,50);
					
					if ( dwWaitRes == WAIT_OBJECT_0 )
					{
						bCanRemove = TRUE;
					}
					else
					{
						bCanRemove = FALSE;
					}

				}

				

				if ( bCanRemove )
				{
					if((*it)->hEvent)
					{
						SetEvent((*it)->hEvent);
					}
					else
					{
						SetEvent((*it)->hFile);
					}

					CloseHandle((*it)->hMyEvent);

					OutputDebugStringA((*it)->pRecvBuf);
					OutputDebugStringA("\r\n");

					delete *it;
				}
				else
				{
					TmpTmpTask.push_back(*it);
				}
			}
			
			TmpTask.clear();
			
			for(LIST_TASK_PTR it = TmpTmpTask.begin();it!=TmpTmpTask.end();it++)
			{
				TmpTask.push_back(*it);
			}

			CString strMsgOut;
			strMsgOut.Format(L"******RemainTaskSize: %d \r\n",TmpTask.size());
			OutputDebugStringW(strMsgOut);

			Sleep(1);

		}


	}


	return 0;
}


VOID AddTask(NOTIFY_PARAM *pInfo)
{
	PQUEUE_WAIT_TASK pTlsData = (PQUEUE_WAIT_TASK)TlsGetValue(dwTlsIndex);
	if ( NULL == pTlsData)
	{
		pTlsData = new QUEUE_WAIT_TASK;
		pTlsData->hEventTask = CreateEvent(NULL,FALSE,FALSE,NULL);

		TlsSetValue(dwTlsIndex,pTlsData);
		CreateThread(NULL,0,WaitThread,pTlsData,0,NULL);
	}

	RecordHandle( pInfo->hFile );

	pTlsData->locker.Lock();
	pTlsData->QueueTask.push_back(pInfo);
	SetEvent(pTlsData->hEventTask);
	pTlsData->locker.UnLock();
}

NTSTATUS WINAPI MyNtDeviceIoControlFile(
										HANDLE FileHandle,
										HANDLE Event,
										PVOID ApcRoutine,
										PVOID ApcContext,
										PVOID IoStatusBlock,
										ULONG IoControlCode,
										PVOID InputBuffer,
										ULONG InputBufferLength,
										PVOID OutputBuffer,
										ULONG OutputBufferLength 
										)
{

	if( IoControlCode == AFD_RECV )
	{

			PAFD_INFO AfdInfo = (PAFD_INFO)InputBuffer ; 
			char * pRecvBuf = AfdInfo->BufferArray->buf ; 
			CONST ULONG nRecvBufLen = AfdInfo->BufferArray->len;
			IO_STATUS_BLOCK *pIoStatus = (IO_STATUS_BLOCK *)IoStatusBlock;

			HANDLE hMyEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
			NTSTATUS RecvStatus = pNtDeviceIoControlFile(
				FileHandle,
				hMyEvent/*Event*/,
				ApcRoutine,
				ApcContext,
				IoStatusBlock,
				IoControlCode,
				InputBuffer,
				InputBufferLength,
				OutputBuffer,
				OutputBufferLength
				);  

  			if ( pIoStatus->Status == 259 )
  			{
   				NOTIFY_PARAM *ppp = new NOTIFY_PARAM;
				
				ppp->hMyEvent = hMyEvent;
				ppp->hEvent = Event;
				ppp->hFile = FileHandle;
   				ppp->pRecvBuf = pRecvBuf;
				ppp->nRecvBufLen = nRecvBufLen;
				ppp->pIoStatus = pIoStatus;

				AddTask(ppp);
  			}
			else if( pIoStatus->Status == 0 )
			{
				int a=0;
			}

			return RecvStatus;

			
	}

	return pNtDeviceIoControlFile(
		FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		IoControlCode,
		InputBuffer,
		InputBufferLength,
		OutputBuffer,
		OutputBufferLength
		);  

};


BOOL (WINAPI *pCloseHandle)(
							__in HANDLE hObject 
							) = CloseHandle;
BOOL WINAPI MyCloseHandle(
						  __in HANDLE hObject 
						  )
{
	BOOL TReturn = pCloseHandle(
		hObject
		);

// 	CString strMsgOut;
// 	strMsgOut.Format(L"CloseHandle: 0x%x \r\n",hObject);
// 	OutputDebugStringW(strMsgOut);

	DelHandle(hObject);
	
	return TReturn;
};

int (WINAPI *pclosesocket)(
						   IN SOCKET s
						   ) = closesocket;
int WINAPI Myclosesocket(
						 IN SOCKET s
						 )
{
	int TReturn = pclosesocket(
		s
		);

	DelHandle((HANDLE)s);

	return TReturn;
};

#include <detours.h>
#pragma comment(lib,"detours.lib")


VOID StartHook()
{
	if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES) 
	{
		return;
	}
	pNtDeviceIoControlFile = (TypeNtDeviceIoControlFile)::GetProcAddress(GetModuleHandle(L"ntdll.dll"),"NtDeviceIoControlFile");
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID *)&pNtDeviceIoControlFile,MyNtDeviceIoControlFile);
	DetourAttach((PVOID *)&pCloseHandle,MyCloseHandle);
	DetourAttach((PVOID *)&pclosesocket,Myclosesocket);
	DetourTransactionCommit();
}
