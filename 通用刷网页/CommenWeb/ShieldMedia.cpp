#include "stdafx.h"

#include "HttpRecvParser.h"
#include "RWLock.h"
#include "detours.h"
#pragma comment(lib,"detours.lib")

static BOOL g_bShieldResource = FALSE;

#include <list>
using namespace std;

typedef list<CString> LIST_SHIELD_TYPE;
typedef LIST_SHIELD_TYPE::iterator LIST_SHIELD_TYPE_PTR;

CRWLock rwLocker;
LIST_SHIELD_TYPE  lstShieldTypes;


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
		if ( strContentType.GetLength() > 0 )
		{
			strContentType.MakeLower();

			BOOL bShield = FALSE;

			rwLocker.rlock();
			for (LIST_SHIELD_TYPE_PTR it=lstShieldTypes.begin();it!=lstShieldTypes.end();it++)
			{
				if (strContentType.Find(CStringA(*it)) >= 0)
				{
					bShield = TRUE;
					break;
				}
			}
			rwLocker.unlock();

			if ( bShield )
			{
				char chRecvData[500];
				sprintf_s(chRecvData,500,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",strContentType);

				nNewRecvLen = min(strlen(chRecvData),nRecvBufLen);
				memcpy_s(pRecvData,nNewRecvLen,chRecvData,nNewRecvLen);
				bReplaceData = TRUE;
			}
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
	if(g_bShieldResource)
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
	if(g_bShieldResource)
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


BOOL WINAPI InitShieldResource()
{
	static BOOL bInit = FALSE;
	if ( FALSE == bInit )
	{
		bInit = TRUE;

 		DetourTransactionBegin();
 		DetourUpdateThread(GetCurrentThread());
 		DetourAttach(&(PVOID&)pWSARecv, (PBYTE)MyWSARecv);
 		DetourAttach(&(PVOID&)precv, (PBYTE)Myrecv);
 		DetourTransactionCommit();

	}


	return TRUE;

}

BOOL WINAPI UpdateShieldType( LPCWSTR *pszArrayTypes,int nTypesCount )
{
	rwLocker.wlock();

	lstShieldTypes.clear();

	for (int i=0;i<nTypesCount;i++)
	{
		CString strShieldType;
		strShieldType = pszArrayTypes[i];
		strShieldType.MakeLower();

		lstShieldTypes.push_back(strShieldType);
	}

	rwLocker.unlock();
	
	return FALSE;
}

BOOL WINAPI SetShieldResource(BOOL bSwitchOn)
{
	g_bShieldResource = bSwitchOn;

	return TRUE;
}
