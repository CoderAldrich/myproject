#include "stdafx.h"
#include <detours.h>
#include "HttpSendParser.h"
#include "Cookie∏Ù¿Î/HelpFun.h"

#include <assert.h>

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

typedef NTSTATUS (WINAPI *TypeNtDeviceIoControlFile)(
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
	);


TypeNtDeviceIoControlFile pNtDeviceIoControlFile = NULL;

HWND g_hWndNotify = NULL;
UINT g_nMsgNotify = NULL;


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

		CHttpSendParser parser;
		BOOL bRes = parser.ParseData((const char *)buf,len);

		do 
		{
			if ( FALSE == bRes )
			{
				break;
			}

			if(parser.GetRequestMeth().CompareNoCase("post")!=0)
			{
				break;
			}


			CStringA url = parser.GetParseUrl();
			if (url == "http://vms.ku6.com/api/player_log.php")
			{
				const char *pPostData = buf+parser.GetContentStart();
				int     nPostDataLen = len-parser.GetContentStart();

				char *pchTempBuf = new char[nPostDataLen+1];

				memcpy_s(pchTempBuf,nPostDataLen,pPostData,nPostDataLen);
				pchTempBuf[nPostDataLen] = 0;
			
				CStringA strPostData = pchTempBuf;

				CStringA strDataPart[10];
				int nPartCount = DivisionStringA("&",strPostData,strDataPart,10);
				for (int i=0;i<nPartCount;i++)
				{
					CStringA strTempPart[2];
					DivisionStringA("=",strDataPart[i],strTempPart,2);
					if ( strTempPart[0].CompareNoCase("action") == 0 )
					{
						if ( strTempPart[1].CompareNoCase("pause") == 0 )
						{
							PostMessage(g_hWndNotify,g_nMsgNotify,0,0);
						}
					}
				}

				delete pchTempBuf;
			}
	
		} while (FALSE);
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

VOID StartPauseMonitor(HWND hWndNotify,UINT nNotifyMsg)
{
	if ( g_hWndNotify == NULL )
	{
		g_hWndNotify = hWndNotify;
		g_nMsgNotify = nNotifyMsg;

		pNtDeviceIoControlFile = (TypeNtDeviceIoControlFile)::GetProcAddress(GetModuleHandle(L"ntdll.dll"),"NtDeviceIoControlFile");
		if(pNtDeviceIoControlFile)
		{
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourAttach(&(PVOID&)pNtDeviceIoControlFile, (PBYTE)MyNtDeviceIoControlFile);
			DetourTransactionCommit();
		}

	}

}

