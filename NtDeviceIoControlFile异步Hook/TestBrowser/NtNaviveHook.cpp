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

typedef struct tagNOTIFY_PARAM
{
	HANDLE hMyEvent;
	HANDLE hFile;
	HANDLE hEvent;
	char * pRecvBuf;
	ULONG nRecvBufLen;
	IO_STATUS_BLOCK *pIoStatus;
	HANDLE hTest;
}NOTIFY_PARAM,*PNOTIFY_PARAM;

#include <Shlwapi.h>

VOID HandleRecvData(char *pRecvData,int nRecvDataLen)
{
	if ( nRecvDataLen > 0 )
	{
		int nShowLen = min(700,nRecvDataLen);
		char *pchTmp = new char[nShowLen+1];
		memcpy_s(pchTmp,nShowLen,pRecvData,nShowLen);
		pchTmp[nShowLen] = 0;

		char *pchTitle = StrStrIA(pchTmp,"腾讯首页");
		if (pchTitle)
		{
			pchTitle = StrStrIA(pRecvData,"腾讯首页");
			pchTitle[0]='A';
			pchTitle[1]='B';
			pchTitle[2]='C';
			pchTitle[3]='D';
		}

		//OutputDebugStringA((char *)pchTmp);
		//OutputDebugStringA("\r\n-------------------------------------------------------\r\n");
		delete pchTmp;
	}
}

void CALLBACK WaitOrTimerCallback(PVOID lpParam, BOOLEAN bReason)//事件处理函数
{
	NOTIFY_PARAM *pInfo = (NOTIFY_PARAM *)lpParam;
	if ( NULL == pInfo )
	{
		return;
	}

	if(bReason == FALSE)//reason为FALSE表示为事件响应，为TRUE表示为超时响应
	{
		//此处处理数据
		if ( pInfo->pIoStatus->Status == 0 )
		{
			HandleRecvData( pInfo->pRecvBuf , pInfo->pIoStatus->Information );
		}

		if(pInfo->hEvent)
		{
			SetEvent(pInfo->hEvent);
		}
		else
		{
			SetEvent(pInfo->hFile);
		}
	}



	UnregisterWait(pInfo->hTest);
	CloseHandle(pInfo->hMyEvent);


	delete pInfo;
	pInfo = NULL;

	return ;
}



VOID AddTask(NOTIFY_PARAM *pInfo)
{
	RegisterWaitForSingleObject(&(pInfo->hTest), pInfo->hMyEvent, WaitOrTimerCallback, (LPVOID)pInfo, 20*1000, WT_EXECUTEINPERSISTENTTHREAD);
	int a=0;
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

	/*
	Once the service is complete the Event, 
	if specified, is set to the signaled state.
	If no Event parameter is specified, then the file object specified by the FileHandle is set to the signaled state. 
	If an ApcRoutine is specified, it is invoked with the ApcContext and the IoStatusBlock as its arguments.
	*/
	if( IoControlCode == AFD_RECV )
	{

			PAFD_INFO AfdInfo = (PAFD_INFO)InputBuffer ; 
			char * pRecvBuf = AfdInfo->BufferArray->buf ; 
			CONST ULONG nRecvBufLen = AfdInfo->BufferArray->len;
			IO_STATUS_BLOCK *pIoStatus = (IO_STATUS_BLOCK *)IoStatusBlock;

			HANDLE hMyEvent = NULL;
			if (Event)
			{
				hMyEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
			}

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
			else
			{
				if (hMyEvent)
				{
					CloseHandle(hMyEvent);
				}
				

				if( pIoStatus->Status == 0 )
				{
					HandleRecvData(pRecvBuf,pIoStatus->Information);
				}
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

	//DelHandle(hObject);
	
	return TReturn;
};

int (WINAPI *pclosesocket)(
						   IN SOCKET s
						   ) = closesocket;
int WINAPI Myclosesocket(
						 IN SOCKET s
						 )
{
	int TReturn = pclosesocket(s);
	//DelHandle((HANDLE)s);
	return TReturn;
};

#include <detours.h>
#pragma comment(lib,"detours.lib")


VOID StartHook()
{
	//return ;
// 	if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES) 
// 	{
// 		return;
// 	}
	
// 	pHandleMapLock = new CCSLock;
// 	pHandleMap = new HANDLE_HASH_MAP;

	pNtDeviceIoControlFile = (TypeNtDeviceIoControlFile)::GetProcAddress(GetModuleHandle(L"ntdll.dll"),"NtDeviceIoControlFile");
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach((PVOID *)&pNtDeviceIoControlFile,MyNtDeviceIoControlFile);
	//DetourAttach((PVOID *)&pCloseHandle,MyCloseHandle);
	//DetourAttach((PVOID *)&pclosesocket,Myclosesocket);
	DetourTransactionCommit();
}
