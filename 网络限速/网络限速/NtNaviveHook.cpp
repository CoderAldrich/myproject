#include "stdafx.h"
#include "NtNaviveHook.h"
#include <detours.h>
#pragma comment(lib,"detours.lib")

#include <list>
using namespace std;

#define AFD_RECV 0x12017
#define AFD_SEND 0x1201f
#define STATUS_UNSUCCESSFUL              ((NTSTATUS)0xC0000001L)
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L) // ntsubauth
#define STATUS_DEVICE_NOT_READY          ((NTSTATUS)0xC00000A3L)


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

//临界区互斥锁
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


VOID DebugStringW(const WCHAR* fmt, ...)
{
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


#define DebugStringW(...)

CCSLock g_csLocker;

LONGLONG llMaxSpeed = 100*1024;

LONGLONG llCurRecvLen = 0;

typedef struct _tagTIME_VALUE
{
	DWORD dwTime;
	LONGLONG dwValue;
}TIME_VALUE,*PTIME_VALUE;

typedef list<TIME_VALUE> LIST_TIME_VALUE,*PLIST_TIME_VALUE;
typedef LIST_TIME_VALUE::iterator LIST_TIME_VALUE_PTR;

LIST_TIME_VALUE ListTimeValue;

DWORD WINAPI TickThread( PVOID pParam )
{
	while ( TRUE )
	{
		g_csLocker.Lock();
		
		while (TRUE)
		{
			LIST_TIME_VALUE_PTR it = ListTimeValue.begin();
			if ( it == ListTimeValue.end() )
			{
				break;
			}


			if ( GetTickCount() - it->dwTime > 1000 )
			{
				llCurRecvLen -= it->dwValue;
				ListTimeValue.erase(it);

				DebugStringW(L"llCurRecvLen -- %d\r\n",llCurRecvLen);
			}
			else
			{
				//DebugStringW(L"Break Contine\r\n");
				break;
			}
		}

		g_csLocker.UnLock();

		Sleep(5);
	}
}

class CInitcall
{
public:
	CInitcall()
	{
		CreateThread(NULL,0,TickThread,NULL,0,NULL);
	}
	~CInitcall()
	{

	}
};

CInitcall Initcall;


LONGLONG GetMaxRecvLen( LONGLONG llWantLen )
{
	LONGLONG llCanRecvLen = 0;

	g_csLocker.Lock();

	//判断是否有空余的接受空间
	if ( llMaxSpeed > llCurRecvLen )
	{
		LONGLONG  llTempLen = 0;
		llTempLen = llMaxSpeed - llCurRecvLen;

		llCanRecvLen = min(llTempLen,llWantLen); 
		llCurRecvLen += llCanRecvLen;

		DebugStringW(L"llCurRecvLen ++ %d\r\n",llCurRecvLen);

		TIME_VALUE Value;
		Value.dwTime = GetTickCount();
		Value.dwValue = llCanRecvLen;

		ListTimeValue.push_back(Value);
	}
	else
	{
		//DebugStringW(L"没有多余的接受空间\r\n");
	}

	g_csLocker.UnLock();

	return llCanRecvLen;
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


	if (IoControlCode == AFD_SEND)
	{
		PAFD_INFO AfdInfo = (PAFD_INFO)InputBuffer ; 
		char * buf = AfdInfo->BufferArray->buf ; 
		ULONG len = AfdInfo->BufferArray->len;


	}
	else if( IoControlCode == AFD_RECV )
	{
		NTSTATUS nStats = pNtDeviceIoControlFile(
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

		IO_STATUS_BLOCK *pIoStatus = (IO_STATUS_BLOCK *)IoStatusBlock;
		PAFD_INFO AfdInfo = (PAFD_INFO)InputBuffer ; 
		char * buf = AfdInfo->BufferArray->buf ; 
		ULONG len = AfdInfo->BufferArray->len;
		
		if (nStats == 259)
		{
			WaitForSingleObject(FileHandle,INFINITE);
		}

		if ( pIoStatus->Status /*nStats*/ == STATUS_SUCCESS || nStats == STATUS_SUCCESS )
		{
			LONGLONG llTotalLen = 0;

			while ( llTotalLen < pIoStatus->Information )
			{
				llTotalLen += GetMaxRecvLen( (LONGLONG)(pIoStatus->Information) - llTotalLen );
				Sleep(1);
			}
		}




		return nStats;
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


VOID SetLimiteSpeed( int nMaxSpeed )
{
	pNtDeviceIoControlFile = (TypeNtDeviceIoControlFile)::GetProcAddress(GetModuleHandle(L"ntdll.dll"),"NtDeviceIoControlFile");
	if(pNtDeviceIoControlFile)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach((PVOID *)&pNtDeviceIoControlFile,MyNtDeviceIoControlFile);
		DetourTransactionCommit();
	}

}