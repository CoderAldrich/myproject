// MiniProxyServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TcpSocket.h"
#include "HttpSendParser.h"
#include "HelpFun.h"
#include <atlstr.h>

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

#include "SSLTcpSocket.h";

VOID HandleHttpsConnect( CSSLTcpSocket *pclientsock,CSSLTcpSocket *premotesock )
{

	do
	{
		char chRecvBuffer[4096];
		while (TRUE)
		{
			FD_SET fdRead;
			FD_ZERO( &fdRead );
			FD_SET(pclientsock->GetHandle(),&fdRead);
			FD_SET(premotesock->GetHandle(),&fdRead);
			int nRet = select(0,&fdRead,NULL,NULL,NULL);

			if ( nRet != SOCKET_ERROR )
			{
				//从远程服务器发来的数据
				if(FD_ISSET(premotesock->GetHandle(),&fdRead)!=0)
				{
					int nRecvLen = premotesock->RecvData( chRecvBuffer,4096 );
					if ( SOCKET_ERROR == nRecvLen || 0 == nRecvLen )
					{
						//	DebugStringA("remoteSock Break %d",nRecvLen);
						break;
					}

					//	DebugStringA("remoteSock RecvData %d",nRecvLen);

					int nSendLen = pclientsock->SendData(chRecvBuffer,nRecvLen);
					if ( SOCKET_ERROR == nSendLen || 0 == nSendLen)
					{
						//	DebugStringA("clientSock Break %d",nSendLen);
						break;
					}

					//DebugStringA("clientSock SendData %d",nSendLen);
				}

				//客户机利用已有TCP链接继续请求
				if(FD_ISSET(pclientsock->GetHandle(),&fdRead)!=0)
				{

					int nRecvLen = pclientsock->RecvData( chRecvBuffer,4096 );
					if ( SOCKET_ERROR == nRecvLen || 0 == nRecvLen )
					{
						//DebugStringA("clientSock Break %d",nRecvLen);
						break;
					}

					//DebugStringA("clientSock RecvData %d",nRecvLen);
					
					BOOL bTransmitData = TRUE;
					CHttpSendParser sendparser;
					if(sendparser.ParseData(chRecvBuffer,nRecvLen))
					{
						CStringA strUrl;
						strUrl = sendparser.GetParseUrl();
						DebugStringA("Url:%s",strUrl);

						if ( strUrl.Find("www.baidu.com/?tn=") >= 0 && strUrl.Find("www.baidu.com/?tn=123_pg") < 0 )
						{
							bTransmitData = FALSE;
							LPCSTR pchResponseData = "HTTP/1.1 302 Move\r\nLocation: https://www.baidu.com/?tn=123_pg\r\n\r\n";
							pclientsock->SendData((PVOID)pchResponseData,strlen(pchResponseData));
						}

					}
					
					if(bTransmitData)
					{
						int nSendLen = premotesock->SendData(chRecvBuffer,nRecvLen);
						if ( SOCKET_ERROR == nSendLen || 0 == nSendLen)
						{
							//DebugStringA("remoteSock Break %d",nSendLen);
							break;
						}
						//DebugStringA("remoteSock SendData %d",nSendLen);
					}

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

	return;
}

DWORD WINAPI HttpsRequestHandleThread( PVOID pParam )
{
	CSSLTcpSocket *psslclient = (CSSLTcpSocket *)pParam;
	
	if ( NULL == psslclient )
	{
		return 0;
	}
	
	char chReadBuffer[4096];
	int  nReadTotalLen = 0;
	CStringA strHost;
	while (1)
	{
		int nRet = psslclient->RecvData( chReadBuffer+nReadTotalLen,4096-nReadTotalLen );
		if (nRet == -1)
		{
			break;
		}

		nReadTotalLen+=nRet;

		CHttpSendParser sendparser;
		if(sendparser.ParseData(chReadBuffer,nReadTotalLen))
		{
			strHost = sendparser.GetHost();
			DebugStringA("Url:%s",sendparser.GetParseUrl());
			break;
		}
	}

 	CSSLTcpSocket *psslremote = new CSSLTcpSocket();
 	psslremote->CreateSSLTcpSocketForClient();
 	psslremote->SSLConnect(strHost,443);

 	int nRet = psslremote->SendData((PVOID)chReadBuffer,nReadTotalLen);

	HandleHttpsConnect(psslclient,psslremote);

	return 0;
}

VOID HttpsProxyServer()
{
	CSSLTcpSocket sslsock;
	BOOL bRes = sslsock.CreateSSLTcpSocketForServer( "server.crt","server.key" );
	if ( bRes )
	{
		sslsock.InitAccept(443);

		while (TRUE)
		{
			CSSLTcpSocket *psslclient = sslsock.SSLAccept(NULL);
			if (psslclient)
			{
				CreateThread( NULL,0,HttpsRequestHandleThread,psslclient,0,NULL );
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


	//ProxyRun();


// 	CSSLTcpSocket sslsock;
// 	sslsock.CreateSSLTcpSocketForClient();
// 	sslsock.SSLConnect("www.baidu.com",443);
// 
// 	LPCSTR pchSendData = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\n\r\n";
// 	int nRet = sslsock.SendData((PVOID)pchSendData,strlen(pchSendData));
// 
// 	char chReadBuffer[4096];
// 	sslsock.RecvData(chReadBuffer,4096);
// 	
// 	int a=0;

	HttpsProxyServer();

	Sleep(500);

	getchar();
	return 0;
}

