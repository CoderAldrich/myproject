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

typedef struct _tagCALL_PROXY
{
	HANDLE hFile;
	HANDLE hEvent;
	HANDLE hOrgEvent;
	PVOID  ApcRoutine;
	PVOID ApcContext;
	IO_STATUS_BLOCK	IoStatusBlock;
	IO_STATUS_BLOCK *porgIoStatusBlock;
	BYTE *pRecvBuffer;
	LONGLONG llRecvBufferLen;
	HANDLE hTest;
}CALL_PROXY,*PCALL_PROXY;


void CALLBACK WaitOrTimerCallback(PVOID lpParam, BOOLEAN bReason)//事件处理函数
{
	CALL_PROXY *pCallProxy = (CALL_PROXY *)lpParam;
	if ( NULL == pCallProxy )
	{
		return;
	}

	memcpy_s(pCallProxy->porgIoStatusBlock,sizeof(IO_STATUS_BLOCK),&(pCallProxy->IoStatusBlock),sizeof(IO_STATUS_BLOCK));

	if(bReason == FALSE)//reason为FALSE表示为事件响应，为TRUE表示为超时响应
	{

		if (pCallProxy->hOrgEvent)
		{
			SetEvent(pCallProxy->hOrgEvent);
		}


//  		NTSTATUS RecvStatus = pNtDeviceIoControlFile(
//  			pCallProxy->hFile,
//  			(HANDLE)NULL,
//  			0,
//  			pCallProxy->porgIoStatusBlock,
//  			pCallProxy->porgIoStatusBlock,
//  			0x120EF,
//  			NULL,
//  			0,
//  			0,
//  			0);

		int a=0;
	}

	UnregisterWait(pCallProxy->hTest);
	

	
	CloseHandle(pCallProxy->hEvent);
	delete pCallProxy;
	pCallProxy = NULL;

	return ;
}


DWORD WINAPI TestThread(PVOID lpParam)
{

	CALL_PROXY *pCallProxy = (CALL_PROXY *)lpParam;
	if ( NULL == pCallProxy )
	{
 		return 0;
	}

	WaitForSingleObject(pCallProxy->hEvent,5000);

	memcpy_s(pCallProxy->porgIoStatusBlock,sizeof(IO_STATUS_BLOCK),&(pCallProxy->IoStatusBlock),sizeof(IO_STATUS_BLOCK));

	if (pCallProxy->hOrgEvent)
	{
		SetEvent(pCallProxy->hOrgEvent);
	}
	else
	{
		BOOL bRes = SetEvent(pCallProxy->hFile);
		DWORD dwErrorCode = GetLastError();
		int a=0;
	}

	CloseHandle(pCallProxy->hEvent);
	delete pCallProxy;
	pCallProxy = NULL;

	return 0;
}


VOID AddTask( PCALL_PROXY pProxyInfo )
{
	CreateThread(NULL,0,TestThread,pProxyInfo,0,NULL);
	//RegisterWaitForSingleObject(&(pProxyInfo->hTest), pProxyInfo->hEvent, WaitOrTimerCallback, (LPVOID)pProxyInfo, 5*1000, WT_EXECUTEINPERSISTENTTHREAD);
}

HANDLE g_hFileHandle = NULL;

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

	if ( IoControlCode == 0x120EF )
	{
		int a=0;
	}

	if( IoControlCode == AFD_RECV )
	{

			PAFD_INFO AfdInfo = (PAFD_INFO)InputBuffer ; 
			char * pRecvBuf = AfdInfo->BufferArray->buf ; 
			CONST ULONG nRecvBufLen = AfdInfo->BufferArray->len;
			IO_STATUS_BLOCK *pIoStatus = (IO_STATUS_BLOCK *)IoStatusBlock;

			CALL_PROXY *pCallProxy = new CALL_PROXY;
			pCallProxy->hFile = FileHandle;
			pCallProxy->hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
			pCallProxy->hOrgEvent = Event;
			pCallProxy->ApcRoutine = ApcRoutine;
			pCallProxy->ApcContext = ApcContext;
			ZeroMemory(&(pCallProxy->IoStatusBlock),sizeof(pCallProxy->IoStatusBlock));
			pCallProxy->porgIoStatusBlock = pIoStatus;

			//替换成自己的参数
			NTSTATUS RecvStatus = pNtDeviceIoControlFile(
				FileHandle,
				pCallProxy->hEvent/*Event*/,
				NULL/*ApcRoutine*/,

				NULL,
				//ApcContext,
				//&(pCallProxy->IoStatusBlock),

				&(pCallProxy->IoStatusBlock)/*IoStatusBlock*/,
				IoControlCode,
				InputBuffer,
				InputBufferLength,
				OutputBuffer,
				OutputBufferLength
				);


			if ( RecvStatus == 259 )
			{
				PVOID pData = &(pIoStatus->Status);
				g_hFileHandle = FileHandle;

//  				memcpy_s(IoStatusBlock,sizeof(IO_STATUS_BLOCK),&(pCallProxy->IoStatusBlock),sizeof(IO_STATUS_BLOCK));
//  				AddTask(pCallProxy);

//   				DWORD dwWaitRes = WaitForSingleObject(pCallProxy->hEvent,5000);
//   				if ( WAIT_OBJECT_0 == dwWaitRes )
//   				{
//   
//   					memcpy_s(IoStatusBlock,sizeof(IO_STATUS_BLOCK),&(pCallProxy->IoStatusBlock),sizeof(IO_STATUS_BLOCK));
//   					if ( Event  )
//   					{
//   						SetEvent(Event);
//   					}
//   
//   					RecvStatus = 0;
//   				}
//  				else
//  				{
//  					int a=0;
//  				}
			
			}
			else 
			{
// 				if (Event)
// 				{
// 					SetEvent(Event);
// 				}
				memcpy_s(IoStatusBlock,sizeof(IO_STATUS_BLOCK),&(pCallProxy->IoStatusBlock),sizeof(IO_STATUS_BLOCK));
				CloseHandle(pCallProxy->hEvent);
				delete pCallProxy;
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

BOOL (WSAAPI *pWSAGetOverlappedResult)(
									   IN SOCKET s,
									   IN LPWSAOVERLAPPED lpOverlapped,
									   OUT LPDWORD lpcbTransfer,
									   IN BOOL fWait,
									   OUT LPDWORD lpdwFlags 
									   ) = WSAGetOverlappedResult;
BOOL WSAAPI MyWSAGetOverlappedResult(
									 IN SOCKET s,
									 IN LPWSAOVERLAPPED lpOverlapped,
									 OUT LPDWORD lpcbTransfer,
									 IN BOOL fWait,
									 OUT LPDWORD lpdwFlags 
									 )
{
	if (g_hFileHandle == (HANDLE)s)
	{
		int a=0;
	}
	BOOL TReturn = pWSAGetOverlappedResult(
		s,
		lpOverlapped,
		lpcbTransfer,
		fWait,
		lpdwFlags
		);
	return TReturn;
};

int (WSAAPI *pWSARecv)(
					   IN SOCKET s,
					   LPWSABUF lpBuffers,
					   IN DWORD dwBufferCount,
					   __out_opt LPDWORD lpNumberOfBytesRecvd,
					   IN OUT LPDWORD lpFlags,
					   __in_opt LPWSAOVERLAPPED lpOverlapped,
					   __in_opt LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine 
					   ) = WSARecv;
int WSAAPI MyWSARecv(
					 IN SOCKET s,
					 LPWSABUF lpBuffers,
					 IN DWORD dwBufferCount,
					 __out_opt LPDWORD lpNumberOfBytesRecvd,
					 IN OUT LPDWORD lpFlags,
					 __in_opt LPWSAOVERLAPPED lpOverlapped,
					 __in_opt LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine 
					 )
{
	int TReturn = pWSARecv(
		s,
		lpBuffers,
		dwBufferCount,
		lpNumberOfBytesRecvd,
		lpFlags,
		lpOverlapped,
		lpCompletionRoutine
		);
	return TReturn;
};


PTP_WIN32_IO_CALLBACK pCompletionRoutine = NULL;

VOID WINAPI MyCompletionRoutine(
	__inout     PTP_CALLBACK_INSTANCE Instance,
	__inout_opt PVOID                 Context,
	__inout_opt PVOID                 Overlapped,
	__in        ULONG                 IoResult,
	__in        ULONG_PTR             NumberOfBytesTransferred,
	__inout     PTP_IO                Io
	)
{
	if (NumberOfBytesTransferred)
	{
		int a=0;
	}
	return pCompletionRoutine(
		Instance,
		Context,
		Overlapped,
		IoResult,
		NumberOfBytesTransferred,
		Io
		);
}

PTP_IO (WINAPI *pCreateThreadpoolIo)(
									 _In_ HANDLE fl,
									 _In_ PTP_WIN32_IO_CALLBACK pfnio,
									 _Inout_opt_ PVOID pv,
									 _In_opt_ PTP_CALLBACK_ENVIRON pcbe  
									 ) = CreateThreadpoolIo;
PTP_IO WINAPI MyCreateThreadpoolIo(
								   _In_ HANDLE fl,
								   _In_ PTP_WIN32_IO_CALLBACK pfnio,
								   _Inout_opt_ PVOID pv,
								   _In_opt_ PTP_CALLBACK_ENVIRON pcbe  
								   )
{
	static BOOL bHooked = FALSE;
	if ( FALSE == bHooked )
	{
		 bHooked = TRUE;

		 pCompletionRoutine = pfnio;
		 DetourTransactionBegin();
		 DetourUpdateThread(GetCurrentThread());
		 DetourAttach((PVOID *)&pCompletionRoutine,MyCompletionRoutine);
		 DetourTransactionCommit();

	}
	PTP_IO TReturn = pCreateThreadpoolIo(
		fl,
		pfnio,
		pv,
		pcbe
		);
	return TReturn;
};


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
	DetourAttach((PVOID *)&pWSARecv,MyWSARecv);
	DetourAttach((PVOID *)&pCreateThreadpoolIo,MyCreateThreadpoolIo);
	
	//DetourAttach((PVOID *)&pWSAGetOverlappedResult,MyWSAGetOverlappedResult);
	//DetourAttach((PVOID *)&pWaitForSingleObject,MyWaitForSingleObject);
	//DetourAttach((PVOID *)&pCloseHandle,MyCloseHandle);
	//DetourAttach((PVOID *)&pclosesocket,Myclosesocket);
	DetourTransactionCommit();
}
