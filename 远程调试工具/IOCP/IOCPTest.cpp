#include "stdafx.h"

#include "IOCPExport.h"
#include <atlstr.h>


HANDLE hToClientServer = NULL;


VOID WINAPI ClientConnectCallback( HANDLE hClient,sockaddr_in *psiClient )
{
	BOOL *pbHeartBeatResponse = new BOOL;
	*pbHeartBeatResponse = TRUE;
	IOCPSetClientUserParam(hToClientServer,hClient,pbHeartBeatResponse);
	
}
VOID WINAPI ClientDisConnectCallback( HANDLE hClient , PVOID pUserParam)
{
	delete pUserParam;
}

VOID WINAPI DataRecvCallback( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen)
{
	char chTestData[]="wolegecawolegecawolegecawolegeca";

	IOCPPostSendRequest(hToClientServer,hClient,(BYTE *)chTestData,strlen(chTestData),NULL);
	CStringA strTest;
	strTest.Append((char *)pDataBuffer,dwDataLen);

	if (strTest!="HelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorld")
	{
		int a=0;
	}
}
/////////////////////////////////////////


VOID WINAPI DataRecvCallback( HANDLE hClient, BYTE *pDataBuffer,DWORD dwDatalen )
{
	CStringA strTest;
	strTest.Append((char *)pDataBuffer,dwDatalen);

	if (strTest!="wolegecawolegecawolegecawolegeca")
	{
		int a=0;
	}
}
VOID WINAPI ConnectClosed( HANDLE hClient )
{
	int a=0;
}

#ifndef _WINDLL
int WINAPI WinMain (
		 __in HINSTANCE hInstance,
		 __in_opt HINSTANCE hPrevInstance,
		 __in_opt LPSTR lpCmdLine,
		 __in int nShowCmd
		 )
{
	IOCP_TCP_CALLBACK TcpCallback;
	TcpCallback.pClientConnect = ClientConnectCallback ;
	TcpCallback.pClientDisConnect = ClientDisConnectCallback ;
	TcpCallback.pDataRecv = DataRecvCallback;

	hToClientServer = IOCPCreateTcpServer(8889,&TcpCallback);


	HANDLE hClient = CreateClient(DataRecvCallback,ConnectClosed);
	BOOL bConRes = ClientConnect(hClient,"localhost",8889);
	StartRecvData(hClient);

	while (TRUE)
	{
		char chTestData[]="HelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorld";
		ClientSendData(hClient,(BYTE *)chTestData,strlen(chTestData));

		Sleep(1);
	}

	while (TRUE)
	{
		Sleep(10000);
	}

	return 0;
}

#endif

