// RemoteDebugerServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\iocp\IOCPExport.h"
#pragma comment(lib,"IOCP.lib")
#include "..\公共\ProtocolHandler.h"

HANDLE hDebugerClientServer = NULL;
HANDLE hControllerClientServer = NULL;

VOID WINAPI DebugerClientConnectCallback( HANDLE hClient,sockaddr_in *psiClient )
{

}
VOID WINAPI DebugerClientDisConnectCallback( HANDLE hClient , PVOID pUserParam)
{

}

VOID WINAPI DebugerDataRecvCallback( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen)
{
	CProtocolHandler ptlHandler;
	ptlHandler.ParseProtocolString((LPCSTR)pDataBuffer,dwDataLen);

	HANDLE hCtrlClient = (HANDLE)ptlHandler.GetParamValueInt("target",0);
	if (hCtrlClient)
	{
		IOCPPostSendRequest(hControllerClientServer,hCtrlClient,pDataBuffer,dwDataLen,NULL);
	}
	
}
/////////////////////////////////
VOID WINAPI ControllerConnectCallback( HANDLE hClient,sockaddr_in *psiClient )
{

}
VOID WINAPI ControllerClientDisConnectCallback( HANDLE hClient , PVOID pUserParam)
{

}

VOID WINAPI OnlineClientEnumCallBack( PVOID pParam , HANDLE hClient )
{
	list<HANDLE> *plstHandles = (list<HANDLE> *)pParam;
	plstHandles->push_back(hClient);
}
VOID WINAPI ControllerDataRecvCallback( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen)
{
	CProtocolHandler ptlHandler;
	ptlHandler.ParseProtocolString((LPCSTR)pDataBuffer,dwDataLen);

	CString strCmd;
	strCmd = ptlHandler.GetParamValueString("cmd","");
	if (strCmd == L"getonlineclient")
	{
		CProtocolHandler ptlResponse;

		list<HANDLE> lstHandles;
		IOCPGetAllOnlineClient(hDebugerClientServer,OnlineClientEnumCallBack,&lstHandles);

 		ptlResponse.SetParamValueString("cmd","getonlineclient");
 		ptlResponse.SetParamValueInt("clientcount",lstHandles.size());
 
 		int nIndex = 0;
 		HANDLE hTempHandle = NULL;
 		for (list<HANDLE>::const_iterator it = lstHandles.begin();it!=lstHandles.end();it++)
 		{
 			hTempHandle = *it;
 
 			CStringA strTempKeyName;
 			strTempKeyName.Format("client%d",nIndex);
 			ptlResponse.SetParamValueInt(strTempKeyName,(int)hTempHandle);
 			nIndex++;
 		}
 
 		CStringA strResponse;
 		strResponse = ptlResponse.BuildData();
 
 		IOCPPostSendRequest(hControllerClientServer,hClient,(BYTE *)strResponse.GetBuffer(),strResponse.GetLength(),NULL);
	}

	if (strCmd == L"runcmd")
	{
		HANDLE hTargetClient = (HANDLE)ptlHandler.GetParamValueInt( "target", 0 );
		if (hTargetClient)
		{
			ptlHandler.SetParamValueInt("source",(int)hClient);
			CStringA strSendData;
			strSendData = ptlHandler.BuildData();

			IOCPPostSendRequest(hDebugerClientServer,hTargetClient,(BYTE *)strSendData.GetBuffer(),strSendData.GetLength(),NULL);
		}

	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	IOCP_TCP_CALLBACK TcpCallback;
	TcpCallback.pClientConnect = DebugerClientConnectCallback ;
	TcpCallback.pClientDisConnect = DebugerClientDisConnectCallback ;
	TcpCallback.pDataRecv = DebugerDataRecvCallback;

	hDebugerClientServer = IOCPCreateTcpServer(8889,&TcpCallback);

	TcpCallback.pClientConnect = ControllerConnectCallback ;
	TcpCallback.pClientDisConnect = ControllerClientDisConnectCallback ;
	TcpCallback.pDataRecv = ControllerDataRecvCallback;

	hControllerClientServer = IOCPCreateTcpServer(8890,&TcpCallback);

	while (TRUE)
	{
		Sleep(1000);
	}


	return 0;
}

