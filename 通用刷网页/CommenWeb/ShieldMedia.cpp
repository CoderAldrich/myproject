#include "stdafx.h"

#include "HttpRecvParser.h"

#include "detours.h"
#pragma comment(lib,"detours.lib")

BOOL g_bShieldMedia = FALSE;

BOOL HandleRecvData(SOCKET s,char *pRecvData,const int nRecvLen,const int nRecvBufLen,int &nNewRecvLen)
{

	if( NULL == pRecvData || nRecvBufLen <= 0 || nRecvLen <= 0)
	{
		return FALSE;
	}

	BOOL bReplaceData = FALSE;
	char *pRecvBuf=new char[nRecvLen+1];
	memcpy_s(pRecvBuf,nRecvLen,pRecvData,nRecvLen);
	pRecvBuf[nRecvLen] = 0;

	nNewRecvLen = 0;

	CHttpRecvParser parser;
	if(parser.ParseData(pRecvBuf,nRecvLen))
	{
		CStringA strContentType;
		strContentType = parser.GetValueByName("Content-Type");
		if (
			strContentType.Find("image") >= 0 
			|| strContentType.Find("application") >= 0 
			|| strContentType.Find("text/css") >= 0 
			)
		{
			char chRecvData[500];
			sprintf_s(chRecvData,500,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",strContentType);

			nNewRecvLen = min(strlen(chRecvData),nRecvBufLen);
			memcpy_s(pRecvData,nNewRecvLen,chRecvData,nNewRecvLen);
			bReplaceData = TRUE;
		}
	}

	delete pRecvBuf;

	return bReplaceData;
}

int (WSAAPI *pWSARecv)(
					   SOCKET s,
					   LPWSABUF lpBuffers,
					   DWORD dwBufferCount,
					   LPDWORD lpNumberOfBytesRecvd,
					   LPDWORD lpFlags,
					   LPWSAOVERLAPPED lpOverlapped,
					   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine 
					   ) = WSARecv;
int WSAAPI MyWSARecv(
					 SOCKET s,
					 LPWSABUF lpBuffers,
					 DWORD dwBufferCount,
					 LPDWORD lpNumberOfBytesRecvd,
					 LPDWORD lpFlags,
					 LPWSAOVERLAPPED lpOverlapped,
					 LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine 
					 )
{
	if(g_bShieldMedia)
	{
		int TReturn = pWSARecv(
			s,
			lpBuffers,
			dwBufferCount,
			lpNumberOfBytesRecvd,
			lpFlags,
			0/*lpOverlapped*/,
			0/*lpCompletionRoutine*/
			);


			int nNewRecvLen = 0;
			BOOL bReplaceData = HandleRecvData(s,lpBuffers->buf,*lpNumberOfBytesRecvd,lpBuffers->len,nNewRecvLen);
			if (bReplaceData)
			{
				*lpNumberOfBytesRecvd = nNewRecvLen;
				if (lpOverlapped)
				{
					SetEvent(lpOverlapped->hEvent);
				}
			}
		return TReturn;
	}
	else
	{
		return pWSARecv(
			s,
			lpBuffers,
			dwBufferCount,
			lpNumberOfBytesRecvd,
			lpFlags,
			lpOverlapped,
			lpCompletionRoutine
			);
	}
};


int (WSAAPI *pWSASend)(
					   IN SOCKET s,
					   __in_ecount(dwBufferCount) LPWSABUF lpBuffers,
					   IN DWORD dwBufferCount,
					   __out_opt LPDWORD lpNumberOfBytesSent,
					   IN DWORD dwFlags,
					   __in_opt LPWSAOVERLAPPED lpOverlapped,
					   __in_opt LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine 
					   ) = WSASend;
int WSAAPI MyWSASend(
					 IN SOCKET s,
					 __in_ecount(dwBufferCount) LPWSABUF lpBuffers,
					 IN DWORD dwBufferCount,
					 __out_opt LPDWORD lpNumberOfBytesSent,
					 IN DWORD dwFlags,
					 __in_opt LPWSAOVERLAPPED lpOverlapped,
					 __in_opt LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine 
					 )
{
	int TReturn = pWSASend(
		s,
		lpBuffers,
		dwBufferCount,
		lpNumberOfBytesSent,
		dwFlags,
		lpOverlapped,
		lpCompletionRoutine
		);
	return TReturn;
};



int (PASCAL *precv)(
					IN SOCKET s,
					char FAR * buf,
					IN int len,
					IN int flags
					) = recv;
int PASCAL Myrecv(
				  IN SOCKET s,
				  __out_data_source(NETWORK) char FAR * buf,
				  IN int len,
				  IN int flags
				  )
{
	int TReturn = 0;
	TReturn = precv( s, buf, len, flags );
	if(g_bShieldMedia)
	{
		int nNewRecvLen = 0;
		BOOL bReplaceData = HandleRecvData(s,buf,TReturn,len,nNewRecvLen);
		if (bReplaceData)
		{
			TReturn = nNewRecvLen;
		}
	}

	return TReturn;
};



int (WINAPI *psend)(
					IN SOCKET s,
					IN const char FAR * buf,
					IN int len,
					IN int flags
					) = send;
int WINAPI Mysend(
				  IN SOCKET s,
				  IN const char FAR * buf,
				  IN int len,
				  IN int flags
				  )
{
	int TReturn = psend(
		s,
		buf,
		len,
		flags
		);
	return TReturn;
};


int (WSAAPI *pclosesocket)( IN SOCKET s ) = closesocket;
int WSAAPI Myclosesocket( IN SOCKET s  )
{
	int TReturn = pclosesocket(	s );

	return TReturn;
};


BOOL WINAPI InitShieldMedia()
{
	static BOOL bInit = FALSE;
	if ( FALSE == bInit )
	{
		bInit = TRUE;

 		DetourTransactionBegin();
 		DetourUpdateThread(GetCurrentThread());
 		DetourAttach(&(PVOID&)pWSASend, (PBYTE)MyWSASend);
 		DetourAttach(&(PVOID&)pWSARecv, (PBYTE)MyWSARecv);
 
 		DetourAttach(&(PVOID&)psend, (PBYTE)Mysend);
 		DetourAttach(&(PVOID&)precv, (PBYTE)Myrecv);
 
 		DetourAttach(&(PVOID&)pclosesocket, (PBYTE)Myclosesocket);
 
 		DetourTransactionCommit();

	}


	return TRUE;

}


BOOL WINAPI SetShieldMedia(BOOL bSwitchOn)
{
	g_bShieldMedia = bSwitchOn;

	return TRUE;
}
