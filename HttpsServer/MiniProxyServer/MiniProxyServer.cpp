// MiniProxyServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TcpSocket.h"
#include "HttpSendParser.h"

#include <atlstr.h>

#include <WinInet.h>
#pragma comment(lib,"wininet")

BOOL UploadFile( LPCWSTR pszUploadUrl,LPCWSTR pszHeaders ,SOCKET sockClient )
{
	BOOL bUploadRes = FALSE;

	// for clarity, error-checking has been removed
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	PBYTE     pFileData = NULL;
	do 
	{
		DWORD dwUrlLen = 0;
		if ( NULL == pszUploadUrl || (dwUrlLen = wcslen(pszUploadUrl)) == 0 )
		{
			break;
		}


		CString   strHostName;
		CString   strUrlPath;
		INTERNET_PORT nPort = 80;

		URL_COMPONENTSW UrlComp;
		ZeroMemory(&UrlComp,sizeof(UrlComp));
		UrlComp.dwStructSize = sizeof(UrlComp);
		UrlComp.lpszHostName = strHostName.GetBuffer( MAX_PATH );
		UrlComp.dwHostNameLength = MAX_PATH;
		UrlComp.lpszUrlPath = strUrlPath.GetBuffer(2000);
		UrlComp.dwUrlPathLength = 2000;

		BOOL bCrackRes = InternetCrackUrlW( pszUploadUrl , dwUrlLen , 0 , &UrlComp );

		nPort = UrlComp.nPort;
		strHostName.ReleaseBuffer();
		strUrlPath.ReleaseBuffer();

		if ( FALSE == bCrackRes )
		{
			break;
		}

		hSession = InternetOpen(NULL,INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		if ( NULL == hSession )
		{
			break;
		}

		hConnect = InternetConnect(hSession, strHostName ,
			nPort,
			NULL,
			NULL,
			INTERNET_SERVICE_HTTP,
			0,
			1);

		if ( NULL == hConnect )
		{
			break;
		}

		hRequest = HttpOpenRequestW(hConnect, 
			L"GET",//,L"POST"
			strUrlPath, 
			NULL, 
			NULL, 
			NULL, 
			0, 
			1);

		if( NULL == hRequest )
		{
			break;
		}

		DWORD dwDataSize = 0;
		pFileData = NULL;//BuildPostData(pszFilePath,L"--"+CString(pszBoundary),&dwDataSize);

		bUploadRes = HttpSendRequest(hRequest, 
			pszHeaders, 
			pszHeaders?wcslen(pszHeaders):0, 
			pFileData, 
			dwDataSize);
		
		if ( FALSE == bUploadRes )
		{
			break;
		}
		
		char chReadBuffer[4097];
		DWORD dwReadLen = 0;

		while( InternetReadFile(hRequest,chReadBuffer,4096,&dwReadLen) && dwReadLen > 0 )
		{
			int nRes = send(sockClient,chReadBuffer,dwReadLen,0);
			int a=0;
		}

	} while (FALSE);

	if (pFileData)
	{
		delete pFileData;
	}

	if (hRequest)
	{
		InternetCloseHandle(hRequest);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}


	return bUploadRes;

}

//调试信息输出

#if defined(DEBUG) || defined(_DEBUG)
#include <stdio.h>

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


VOID DebugStringA(const CHAR* fmt, ...)
{
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

#endif

#if defined(DEBUG) || defined(_DEBUG)
#define DebugStringEx(a,b) DebugStringA("%s",__FUNCTION__); DebugString(a, b);

#ifdef UNICODE
#define DebugString DebugStringW
#else
#define DebugString DebugStringA
#endif

VOID DebugStringW(const WCHAR* fmt, ...);
VOID DebugStringA(const CHAR* fmt, ...);

#else
#define DebugString(a,b) 
#define DebugStringW(a,b)
#define DebugStringA(a,b)
#define DebugStringEx(a,b)

#endif

DWORD WINAPI ClientConnectRecvDataThread( PVOID pParam )
{
	CTcpSocket clientSock;
	clientSock.Attach((SOCKET)pParam);

	char chRecvBuf[4096];
	int  nRecvlen = 0;
	while ( TRUE )
	{
		nRecvlen = clientSock.RecvData(chRecvBuf,4096);
		if (nRecvlen <= 0)
		{
			break;
		}

		int a=0;
	}

	
	return 0;
}

VOID HandleConnect( SOCKET sockClient,SOCKET sockRemote )
{
	CTcpSocket clientSock;
	CTcpSocket remoteSock;
	do
	{
		clientSock.Attach(sockClient);
		remoteSock.Attach(sockRemote);

		char chRecvBuffer[4096];
		while (TRUE)
		{
			FD_SET fdRead;
			FD_ZERO( &fdRead );
			FD_SET(clientSock.GetHandle(),&fdRead);
			FD_SET(remoteSock.GetHandle(),&fdRead);
			int nRet = select(0,&fdRead,NULL,NULL,NULL);

			if ( nRet != SOCKET_ERROR )
			{
				//从远程服务器发来的数据
				if(FD_ISSET(remoteSock.GetHandle(),&fdRead)!=0)
				{
					int nRecvLen = remoteSock.RecvData( chRecvBuffer,4096 );
					if ( SOCKET_ERROR == nRecvLen || 0 == nRecvLen )
					{
						//	DebugStringA("remoteSock Break %d",nRecvLen);
						break;
					}

					//	DebugStringA("remoteSock RecvData %d",nRecvLen);

					int nSendLen = clientSock.SendData(chRecvBuffer,nRecvLen);
					if ( SOCKET_ERROR == nSendLen || 0 == nSendLen)
					{
						//	DebugStringA("clientSock Break %d",nSendLen);
						break;
					}

					//DebugStringA("clientSock SendData %d",nSendLen);
				}

				//客户机利用已有TCP链接继续请求
				if(FD_ISSET(clientSock.GetHandle(),&fdRead)!=0)
				{

					int nRecvLen = clientSock.RecvData( chRecvBuffer,4096 );
					if ( SOCKET_ERROR == nRecvLen || 0 == nRecvLen )
					{
						//DebugStringA("clientSock Break %d",nRecvLen);
						break;
					}

					//DebugStringA("clientSock RecvData %d",nRecvLen);

					CHttpSendParser sendparser;
					if(sendparser.ParseData(chRecvBuffer,nRecvLen))
					{
						DebugStringA("Url:%s",sendparser.GetParseUrl());
					}

					int nSendLen = remoteSock.SendData(chRecvBuffer,nRecvLen);
					if ( SOCKET_ERROR == nSendLen || 0 == nSendLen)
					{
						//DebugStringA("remoteSock Break %d",nSendLen);
						break;
					}
					//DebugStringA("remoteSock SendData %d",nSendLen);
				}

				//DebugStringA("--------------------------------------");

			}
			else
			{
				DebugStringA("select read error %d",WSAGetLastError());
			}
		}
	}
	while(FALSE);

	DebugStringA("remoteSock Closed");

	clientSock.Detach();
	remoteSock.Detach();
}


DWORD WINAPI RequestHandleThread(PVOID pParam)
{
	CTcpSocket clientSock;
	CTcpSocket remoteSock;

	clientSock.Attach((SOCKET)pParam);

 	CStringA strRequestHead;
 	CStringA strHost;
 	int nRes = 0;
 	char chRecvBuf[4097];
	FD_SET fdRead;
	FD_ZERO(&fdRead);
	FD_SET(clientSock.GetHandle(),&fdRead);
 	while( TRUE )
 	{
		int nRes = select(0,&fdRead,NULL,NULL,NULL);
		
		nRes = clientSock.RecvData(chRecvBuf,4096);
 		chRecvBuf[nRes] = 0;
 		strRequestHead+=chRecvBuf;
 
 		if ( strRequestHead.Find("\r\n\r\n") >= 0 )
 		{
 			CHttpSendParser sendparser;
 			if(sendparser.ParseData(strRequestHead.GetBuffer(),strRequestHead.GetLength()))
 			{
 				strHost = sendparser.GetHost();
 				DebugStringA("Host:%s",strHost);
 			}
 			break;
 		}
 	}
 	
	do 
	{
		if(!remoteSock.CreateTcpSocket())
		{
			DebugStringA("remoteSock CreateTcpSocket Failed %d",WSAGetLastError());
			break;
		}

		if(!remoteSock.Connect(strHost,80))
		{
			DebugStringA("remoteSock Connect Failed %d",WSAGetLastError());
			break;
		}

		int nRes = remoteSock.SendData(strRequestHead.GetBuffer(),strRequestHead.GetLength());
		if ( nRes == SOCKET_ERROR )
		{
			DebugStringA("remoteSock SendData Failed %d",WSAGetLastError());
			break;
		}

		HandleConnect( clientSock.GetHandle(),remoteSock.GetHandle() );

	} while (FALSE);

	clientSock.CloseTcpSocket();
	remoteSock.CloseTcpSocket();

	return 0;
}
// 导出函数
VOID WINAPI ProxyRun()
{
	CTcpSocket sockListen;
	BOOL bRes = sockListen.CreateTcpSocket();
	if (bRes)
	{
		bRes = sockListen.InitAccept(80);
		if (bRes)
		{
			while (TRUE)
			{
				SOCKET sockclient = sockListen.Accept(NULL);

				DebugStringA("New Client Connected Socket:0x%x",sockclient);

				CreateThread(NULL,0,RequestHandleThread,(PVOID)sockclient,0,NULL);
			}
		}

	}
}


#pragma comment(lib,"urlmon")
int _tmain(int argc, _TCHAR* argv[])
{

// 	CTcpSocket tcp1;
// 	CTcpSocket tcp2;
// 	tcp1.CreateTcpSocket();
// 	tcp2.CreateTcpSocket();
// 
// 	tcp1.Connect("sjz.58.com",80);
// 	tcp2.Connect("sjz.58.com",80);
// 
// 	LPCSTR pchSendData="GET / HTTP/1.1\r\nHost: sjz.58.com\r\n\r\n";
// 	tcp1.SendData((PVOID)pchSendData,strlen(pchSendData));
// 	tcp2.SendData((PVOID)pchSendData,strlen(pchSendData));
// 
// 	FD_SET fdRead;
// 	FD_ZERO( &fdRead );
// 
// 	FD_SET(tcp1.GetHandle(),&fdRead);
// 	FD_SET(tcp2.GetHandle(),&fdRead);
// 	
// 
// 	while ( TRUE )
// 	{
// 		char chReadBuffer[500];
// 		int nRet = select(0,&fdRead,NULL,NULL,NULL);
// 		
// 		if(FD_ISSET(tcp1.GetHandle(),&fdRead)!=0)
// 		{
// 			tcp1.RecvData(chReadBuffer,500);
// 			int a=0;
// 		}
// 
// 		if(FD_ISSET(tcp2.GetHandle(),&fdRead)!=0)
// 		{
// 			tcp2.RecvData(chReadBuffer,500);
// 			int a=0;
// 		}
// 	}


	ProxyRun();
	Sleep(500);

	getchar();
	return 0;
}

