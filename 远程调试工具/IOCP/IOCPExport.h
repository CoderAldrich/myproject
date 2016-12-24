#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <list>
using namespace std;

typedef VOID (WINAPI *CALLBACK_CLIENT_CONNECT)( HANDLE hClient,sockaddr_in *psiClient );
typedef VOID (WINAPI *CALLBACK_CLIENT_DISCONNECT)( HANDLE hClient,PVOID pUserParam);
typedef VOID (WINAPI *CALLBACK_DATA_RECV)( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen);

typedef struct tagIOCP_TCP_CALLBACK
{
	//客户端连接回调
	CALLBACK_CLIENT_CONNECT pClientConnect;
	//客户端断开回调
	CALLBACK_CLIENT_DISCONNECT pClientDisConnect;
	//收到客户端数据回调
	CALLBACK_DATA_RECV  pDataRecv;

}IOCP_TCP_CALLBACK,*PIOCP_TCP_CALLBACK;

typedef VOID (WINAPI *TypeOnlineClientEnumCallBack)( PVOID pParam , HANDLE hClient );


HANDLE IOCPCreateTcpServer( UINT nListenPort , PIOCP_TCP_CALLBACK pTcpCallbacks);
BOOL   IOCPPostSendRequest( HANDLE hIOCPServer,HANDLE hClient, BYTE *pSendBuf,DWORD dwDataLen , DWORD *pdwPenddingSendLen );
BOOL   IOCPDestoryClient( HANDLE hIOCPServer,HANDLE hClient );
BOOL   IOCPSetClientUserParam( HANDLE hIOCPServer,HANDLE hClient,PVOID pUserParam );
PVOID  IOCPGetClientUserParam( HANDLE hIOCPServer,HANDLE hClient );
BOOL   IOCPGetAllOnlineClient( HANDLE hIOCPServer,TypeOnlineClientEnumCallBack pCallback , PVOID pParam );
HANDLE IOCPCreateClient( HANDLE hIOCPServer ,PIOCP_TCP_CALLBACK pTcpCallbacks );
HANDLE IOCPCreateClientWithSock( HANDLE hIOCPServer ,SOCKET sock ,PIOCP_TCP_CALLBACK pTcpCallbacks  );
BOOL   IOCPJoin( HANDLE hIOCPServer,HANDLE hClient );


/////////////////////////////////////////////////

typedef VOID (WINAPI *TypeDataRecvCallback)( HANDLE hClient, BYTE *pDataBuffer,DWORD dwDatalen );
typedef VOID (WINAPI *TypeConnectClosed)( HANDLE hClient );

HANDLE CreateClient( TypeDataRecvCallback pDataRecv,TypeConnectClosed pConnectClosed );
VOID  DeleteClient( HANDLE hClient );
BOOL ClientConnect( HANDLE hClient,LPCSTR pszRemoteAddr,int nRemotePort );
BOOL StartRecvData( HANDLE hClient );
BOOL ClientSendData( HANDLE hClient,BYTE *pDataBuffer,int nDataLen );