#pragma once
#include <WinSock2.h>
#include <Windows.h>


typedef enum IOCP_OPTION_TYPE{
	IOT_ERROR=0,
	IOT_SEND=1,
	IOT_RECV=2
}IOCP_OPTION_TYPE;

typedef struct tagWSAOVERLAPPEDEX{
	WSAOVERLAPPED wsaOverLapped;
	IOCP_OPTION_TYPE IOCPType;
}WSAOVERLAPPEDEX,*PWSAOVERLAPPEDEX;


typedef VOID (WINAPI *CALLBACK_CLIENT_CONNECT)( HANDLE hClient,sockaddr_in *psiClient );
typedef VOID (WINAPI *CALLBACK_CLIENT_DISCONNECT)( HANDLE hClient,PVOID pUserParam);
typedef VOID (WINAPI *CALLBACK_DATA_RECV)( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen);
typedef BOOL (WINAPI *SEND_DATA)( HANDLE hClient,BYTE *pDataBuffer,DWORD dwDataLen);

typedef struct tagIOCP_TCP_CALLBACK
{
	//客户端连接回调
	CALLBACK_CLIENT_CONNECT pClientConnect;
	//客户端断开回调
	CALLBACK_CLIENT_DISCONNECT pClientDisConnect;
	//收到客户端数据回调
	CALLBACK_DATA_RECV  pDataRecv;

}IOCP_TCP_CALLBACK,*PIOCP_TCP_CALLBACK;

class CIOCPTcpClient
{
protected:
	SOCKET m_sock;
	CHAR   m_chRecvBuffer[4096];
	PVOID  m_pUserParam;

	WSABUF wsaBufferRecv;
	WSABUF wsaBufferSend;

	IOCP_TCP_CALLBACK m_Callbacks;
public:
	CIOCPTcpClient(void);
	~CIOCPTcpClient(void);

	VOID SetCallbacks(PIOCP_TCP_CALLBACK pCallbacks);

	BOOL Attach( SOCKET sock );
	BOOL Create();
	SOCKET GetSocketHandle();
	BOOL Connect( LPCSTR pszTargetIP,USHORT nTargetPort);
	
	VOID SetUserParam( PVOID pUserParam );
	PVOID GetUserParam(  );
	
	BOOL PostRecvRequest( );
	BOOL PostSendRequest( BYTE *pSendBuf,DWORD dwDataLen , DWORD *pdwPenddingSendLen );

	VOID OnDataTransfer( HANDLE hYou , PWSAOVERLAPPEDEX pOverLappedEx , DWORD dwBitLen );
	VOID OnDataRecv( HANDLE hYou ,DWORD dwBitLen );
	VOID OnDataSend( HANDLE hYou ,DWORD dwBitLen );
	
	VOID OnConnectClose( HANDLE hYou );
};
