// MiniProxyServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TcpSocket.h"
#include "HttpSendParser.h"

#include <atlstr.h>

#include <WinInet.h>
#pragma comment(lib,"wininet")

BOOL UploadFile( LPCWSTR pszUploadUrl,LPCWSTR pszHeaders )
{
	BOOL bUploadRes = FALSE;

	// for clarity, error-checking has been removed
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	PBYTE     pFileData = NULL;
	PBYTE     pRecvBuffer = NULL;

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
			chReadBuffer[dwReadLen] = 0;
			if ( NULL == pRecvBuffer )
			{
				pRecvBuffer+=L"";
			}
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


DWORD WINAPI RequestHandleThread(PVOID pParam)
{
	CTcpSocket sock;
	sock.Attach((SOCKET)pParam);

	CStringA strRequestHead;

	int nRes = 0;
	char chRecvBuf[100];
	while( (nRes = sock.RecvData(chRecvBuf,99)) > 0 )
	{
		chRecvBuf[nRes] = 0;
		//OutputDebugStringA(chRecvBuf);

		strRequestHead+=chRecvBuf;

		if ( strRequestHead.Find("\r\n\r\n") >= 0 )
		{
			break;
		}
	}

	BOOL bResponseRes = FALSE;

	CHttpSendParser parser;
	BOOL bPasRes = parser.ParseData(strRequestHead.GetBuffer(),strRequestHead.GetLength());
	if (bPasRes)
	{
		CStringA strAppendHeaders;
		int nIndex = 0;
		
		do
		{
			CStringA strTempName;
			CStringA strTempValue;
			strTempValue = parser.GetValueByIndex( nIndex , strTempName );
			if ( strTempName.GetLength() == 0 )
			{
				break;
			}

			strAppendHeaders+=strTempName;
			strAppendHeaders+=": ";
			strAppendHeaders+=strTempValue;
			strAppendHeaders+="\r\n";
			
			nIndex++;
		}while( TRUE );
		

		CStringA strRequestUrl = parser.GetParseUrl();
		OutputDebugStringA(strRequestUrl+"\r\n");

		UploadFile( CString(strRequestUrl),CString(strAppendHeaders) );

		int a=0;
	}


	//sock.SendData((PVOID)chResponseData,strlen(chResponseData));

	sock.CloseTcpSocket();

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
				CreateThread(NULL,0,RequestHandleThread,(PVOID)sockclient,0,NULL);
			}
		}

	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	ProxyRun();
	getchar();
	return 0;
}

