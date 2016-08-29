// HomePopup.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "TcpSocket.h"
#include "HttpSendParser.h"
#include "UrlParser.h"

#include <atlstr.h>

CStringA g_strRedirectUrl;

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
		OutputDebugStringA(chRecvBuf);

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
		CStringA strRequestUrl = parser.GetParseUrl();
		
		if ( strRequestUrl.GetLength() > 0 )
		{
			CUrlParser urlparser;
			urlparser.SetUrl(strRequestUrl);
			if(urlparser.ParseUrl())
			{
 				CStringA strOpenUrl;
 				urlparser.GetParamValueByName("url",strOpenUrl);
				int a=0;
			}
		}
	}

	char HtmlContent[2000];
	char pTempTcpData[4000];
	//style=\"visibility: hidden\"
	sprintf_s(HtmlContent,2000,"<html><head></head><body  scroll=no style=\"margin:0;padding:0\"><iframe src=\"%s\" width=100%% height=100%% frameborder=0></body></html>",g_strRedirectUrl);

	sprintf_s(pTempTcpData,4000,
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: %d\r\n"
		"Connection: Close\r\n\r\n%s",
		strlen(HtmlContent),
		HtmlContent
		);

	sock.SendData((PVOID)pTempTcpData,strlen(pTempTcpData));

	sock.CloseTcpSocket();

	return 0;
}

typedef struct  SERVER_START_PARAM
{
	HANDLE hEvent;
	BOOL   bStartRes;
}SERVER_START_PARAM,*PSERVER_START_PARAM;

DWORD WINAPI ServerAcceptThread( PVOID pParam )
{
	PSERVER_START_PARAM pStartParam = (PSERVER_START_PARAM)pParam;

	pStartParam->bStartRes = FALSE;
	CTcpSocket sockListen;
	do
	{
		BOOL bRes = sockListen.CreateTcpSocket();
		if ( FALSE == bRes)
		{
			break;
		}

		sockListen.InitAccept(80);

		if( 80 != sockListen.GetAcceptPort() )
		{
			break;
		}

		pStartParam->bStartRes = TRUE;
		SetEvent(pStartParam->hEvent);

		while (TRUE)
		{
			SOCKET sockclient = sockListen.Accept(NULL);
			CreateThread(NULL,0,RequestHandleThread,(PVOID)sockclient,0,NULL);
		}

	}while(FALSE);

	SetEvent(pStartParam->hEvent);
	sockListen.CloseTcpSocket();

	return 0;

}

// 导出函数
BOOL StartWebServer(LPCWSTR pszRedirectUrl)
{
	BOOL bStartRes = FALSE;
	SERVER_START_PARAM StartParam;

	g_strRedirectUrl = pszRedirectUrl;

	StartParam.bStartRes = FALSE;
	StartParam.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	CreateThread(NULL,0,ServerAcceptThread,&StartParam,0,NULL);

	DWORD dwWaitRes = WaitForSingleObject(StartParam.hEvent,5000);
	if ( dwWaitRes == WAIT_OBJECT_0 )
	{
		bStartRes = StartParam.bStartRes;
	}

	CloseHandle(StartParam.hEvent);

	return bStartRes;

}