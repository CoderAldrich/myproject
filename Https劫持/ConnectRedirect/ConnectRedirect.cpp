// ConnectRedirect.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <WinSock2.h>
#include <Windows.h>
#include <winternl.h>
#include <detours.h>
#include <WS2tcpip.h>
#include <atlstr.h>

#pragma comment(lib,"ws2_32")
#pragma comment(lib,"detours.lib")

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

#define AFD_CONNECT 0x000120c7
#define AFD_CONNECT1 0x00012007
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

#pragma pack(1) 
typedef struct tagCONNECT_INFO
{
	USHORT usPort;
	DWORD  dwIp;
}CONNECT_INFO,*PCONNECT_INFO;
#pragma pack()

typedef struct _AFD_CONNECT_INFO {
	PVOID UseSAN;
	PVOID Root;
	PVOID ConnectEndpoint;
	CONNECT_INFO RemoteAddress;
} AFD_CONNECT_INFO , *PAFD_CONNECT_INFO;

#pragma pack(1) 
typedef struct _AFD_CONNECT_INFO1 {
	PVOID UseSAN;
	PVOID Root;
	PVOID ConnectEndpoint;
	char chUseLess[8];
	CONNECT_INFO RemoteAddress;
} AFD_CONNECT_INFO1 , *PAFD_CONNECT_INFO1;
#pragma pack() 


CStringA strBaiduIps[10];
int      nBaiduIpCount = 0;

BOOL CheckBaiduIP( LPCSTR pszIp )
{
	BOOL bBaiduIp = FALSE;

	for (int i=0;i<nBaiduIpCount;i++)
	{
		if ( strBaiduIps[i].CompareNoCase(pszIp) == 0 )
		{
			OutputDebugStringW(L"Found Record");
			bBaiduIp = TRUE;
			break;
		}
	}

	return bBaiduIp;
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
	if (IoControlCode == AFD_CONNECT)
	{
		OutputDebugStringW(L"IoControlCode == AFD_CONNECT");
		PAFD_CONNECT_INFO pConnectInfo = (PAFD_CONNECT_INFO)InputBuffer;

		USHORT nPort = ntohs(pConnectInfo->RemoteAddress.usPort);
		char *pchIpAddr = inet_ntoa(*(struct in_addr *)&(pConnectInfo->RemoteAddress.dwIp));
		
		CStringA strIpAddr;
		strIpAddr = pchIpAddr;

		CStringA strMsgOut;
		strMsgOut.Format("%s:%d",strIpAddr,nPort);
		OutputDebugStringA(strMsgOut);

		if ( nPort == 80 )
		{
			pConnectInfo->RemoteAddress.usPort = htons(80);
			pConnectInfo->RemoteAddress.dwIp = ( inet_addr("127.0.0.1") );
		}
	}

	if (IoControlCode == AFD_CONNECT1)
	{
		PAFD_CONNECT_INFO1 pConnectInfo = (PAFD_CONNECT_INFO1)InputBuffer;

		USHORT nPort = ntohs(pConnectInfo->RemoteAddress.usPort);
		char *pchIpAddr = inet_ntoa(*(struct in_addr *)&(pConnectInfo->RemoteAddress.dwIp));

		CStringA strIpAddr;
		strIpAddr = pchIpAddr;

		if (nPort != 53)
		{
			CStringA strMsgOut;
			strMsgOut.Format("AFD_CONNECT1 %s:%d",strIpAddr,nPort);
			OutputDebugStringA(strMsgOut);
		}

		if ( CheckBaiduIP(strIpAddr) && (443 == nPort) )
		{
			OutputDebugStringW(L"Connect Redirect");
			pConnectInfo->RemoteAddress.usPort = htons(553);
			pConnectInfo->RemoteAddress.dwIp = ( inet_addr("127.0.0.1") );
		}
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

typedef INT (WSAAPI *TypeGetAddrInfoExW)(
	PCWSTR pName,
	PCWSTR pServiceName,
	DWORD dwNameSpace,
	LPGUID lpNspId,
	const ADDRINFOEXW *hints,
	PADDRINFOEXW * ppResult,
struct timeval *timeout,
	LPOVERLAPPED lpOverlapped,
	LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine,
	LPHANDLE lpHandle
	);
typedef INT (WSAAPI *TypeGetAddrInfoW)(
									   PCWSTR              pNodeName,
									   PCWSTR              pServiceName,
									   const ADDRINFOW *   pHints,
									   PADDRINFOW *        ppResult
									   );

TypeGetAddrInfoExW pGetAddrInfoExW = NULL;
TypeGetAddrInfoW  pGetAddrInfoW = NULL;

BOOL CheckDomain(CString strDomain,CString &strHostIp)
{
	BOOL bFoundRecord = FALSE;
	
	OutputDebugStringW(strDomain);

	if ( strDomain.CompareNoCase(L"www.baidu.com") == 0 )
	{
		OutputDebugStringW(L"替换DNS");

		strHostIp = L"244.244.244.244";
		bFoundRecord = TRUE;
	}

	return bFoundRecord;
}

void ReplaceDNS(LPCTSTR pszNodeName,PADDRINFOW *ppResult)
{
	CString strHostIP;
	if ( CheckDomain(pszNodeName,strHostIP))
	{
		in_addr ia;
		ia.s_addr = inet_addr(CStringA(strHostIP).GetBuffer());
		for (ADDRINFOW *cur = *ppResult; cur != NULL; cur = cur->ai_next) 
		{
			//替换
			SOCKADDR_IN * psi = (SOCKADDR_IN *)(cur->ai_addr);
			psi->sin_addr.s_addr = ia.s_addr;
		}
	}
}

INT WSAAPI MyGetAddrInfoW(
						  __in_opt        PCWSTR              pNodeName,
						  __in_opt        PCWSTR              pServiceName,
						  __in_opt        const ADDRINFOW *   pHints,
						  __deref_out     PADDRINFOW *        ppResult
						  )
{
	INT nRes = 0;
	if(pGetAddrInfoW)
	{
		nRes = pGetAddrInfoW(
			pNodeName,
			pServiceName,
			pHints,
			ppResult
			);
		ReplaceDNS((LPCTSTR)pNodeName,ppResult);
	}

	return nRes;
}



INT WSAAPI MyGetAddrInfoExW(
							PCWSTR pName,
							PCWSTR pServiceName,
							DWORD dwNameSpace,
							LPGUID lpNspId,
							const ADDRINFOEXW *hints,
							__deref_out PADDRINFOEXW * ppResult,
struct timeval *timeout,
	LPOVERLAPPED lpOverlapped,
	LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine,
	__out_opt LPHANDLE lpHandle
	)
{
	int ires = pGetAddrInfoExW(
		pName,
		pServiceName,
		dwNameSpace,
		lpNspId,
		hints,
		ppResult,
		timeout,
		lpOverlapped,
		lpCompletionRoutine,
		lpHandle
		);

	ReplaceDNS((LPCTSTR)pName,(PADDRINFOW *)ppResult);

	return ires;
}

int (WSAAPI *pWSAConnect)(
						  IN SOCKET s,
						  __in_bcount(namelen) const struct sockaddr FAR * name,
						  IN int namelen,
						  __in_opt LPWSABUF lpCallerData,
						  __out_opt LPWSABUF lpCalleeData,
						  __in_opt LPQOS lpSQOS,
						  __in_opt LPQOS lpGQOS 
						  ) = WSAConnect;
int WSAAPI MyWSAConnect(
						IN SOCKET s,
						__in_bcount(namelen) const struct sockaddr FAR * name,
						IN int namelen,
						__in_opt LPWSABUF lpCallerData,
						__out_opt LPWSABUF lpCalleeData,
						__in_opt LPQOS lpSQOS,
						__in_opt LPQOS lpGQOS 
						)
{
	sockaddr_in *pia = (sockaddr_in *)name;
	if (pia)
	{
		USHORT nPort = ntohs(pia->sin_port);
		char *pchIpAddr = inet_ntoa(pia->sin_addr);
		
		//if (nPort != 53)
		{
			CStringA strMsgOut;
			strMsgOut.Format("WSAConnect %s:%d",pchIpAddr,nPort);
			OutputDebugStringA(strMsgOut);
		}
	}

	int TReturn = pWSAConnect(
		s,
		name,
		namelen,
		lpCallerData,
		lpCalleeData,
		lpSQOS,
		lpGQOS
		);
	return TReturn;
};

int (WINAPI *pconnect)(
					   IN SOCKET s,
					   IN const struct sockaddr FAR *name,
					   IN int namelen
					   ) = connect;
int WINAPI Myconnect(
					 IN SOCKET s,
					 IN const struct sockaddr FAR *name,
					 IN int namelen
					 )
{
	sockaddr_in *pia = (sockaddr_in *)name;
	if (pia)
	{
		USHORT nPort = ntohs(pia->sin_port);
// 		char *pchIpAddr = inet_ntoa(pia->sin_addr);
// 
// 		if (nPort != 53)
// 		{
// 			CStringA strMsgOut;
// 			strMsgOut.Format("connect %s:%d",pchIpAddr,nPort);
// 			OutputDebugStringA(strMsgOut);
// 		}

		if ( 80 == nPort )
		{
			if (
				!(
				pia->sin_addr.S_un.S_un_b.s_b1 == 127 &&
				pia->sin_addr.S_un.S_un_b.s_b2 == 0 &&
				pia->sin_addr.S_un.S_un_b.s_b3 == 0 &&
				pia->sin_addr.S_un.S_un_b.s_b4 == 1

				)
				)
			{
				OutputDebugStringW(L"Myconnect Connect Redirect");
				pia->sin_port = htons(8080);
				pia->sin_addr.s_addr = ( inet_addr("127.0.0.1") );
			}	
		}

	}

	int TReturn = pconnect(
		s,
		name,
		namelen
		);
	return TReturn;
};

BOOL StartConnectRedirectHook()
{
	struct hostent *remoteHost;
	struct in_addr addr;

	remoteHost = gethostbyname("www.baidu.com");
	if (remoteHost)
	{
		if (remoteHost->h_addrtype == AF_INET)
		{
			int i=0;
			while (remoteHost->h_addr_list[i] != 0) 
			{
				addr.s_addr = *(u_long *) remoteHost->h_addr_list[i++];
				strBaiduIps[nBaiduIpCount++] = inet_ntoa(addr);
			}
		}
	}

	strBaiduIps[nBaiduIpCount++]="244.244.244.244";

	for (int i=0;i<nBaiduIpCount;i++)
	{
		OutputDebugStringA("BaiduIp:"+strBaiduIps[i]);
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
// 	pNtDeviceIoControlFile = (TypeNtDeviceIoControlFile)::GetProcAddress(GetModuleHandle(L"ntdll.dll"),"NtDeviceIoControlFile");
// 	if(pNtDeviceIoControlFile)
// 	{
// 		OutputDebugStringW(L"NtDeviceIoControlFile Hook");
// 		DetourAttach(&(PVOID&)pNtDeviceIoControlFile, (PBYTE)MyNtDeviceIoControlFile);
// 	}

// 	pGetAddrInfoW = (TypeGetAddrInfoW)GetProcAddress(LoadLibraryW(L"ws2_32.dll"),"GetAddrInfoW");
// 	if(pGetAddrInfoW != NULL)
// 	{
// 		OutputDebugStringW(L"GetAddrInfoW Hook");
// 		DetourAttach(&(PVOID&)pGetAddrInfoW, (PBYTE)MyGetAddrInfoW);
// 	}
// 	pGetAddrInfoExW = (TypeGetAddrInfoExW)GetProcAddress(LoadLibraryW(L"ws2_32.dll"),"GetAddrInfoExW");
// 	if(pGetAddrInfoExW != NULL)
// 	{
// 		OutputDebugStringW(L"GetAddrInfoExW Hook");
// 		DetourAttach(&(PVOID&)pGetAddrInfoExW, (PBYTE)MyGetAddrInfoExW);
// 	}

//	DetourAttach(&(PVOID&)pWSAConnect, (PBYTE)MyWSAConnect);
	DetourAttach(&(PVOID&)pconnect, (PBYTE)Myconnect);

	DetourTransactionCommit();

	OutputDebugStringW(L"Hook完成");

	return FALSE;
}