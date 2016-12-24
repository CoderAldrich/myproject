// RemoteDebugerServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "..\iocp\IOCPExport.h"
#pragma comment(lib,"IOCP.lib")
#include "..\公共\MemIni.h"

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
	CMemIniFile Ini;
	Ini.ParseMemoryDataW(pDataBuffer,dwDataLen);

	CString strCmd;
	strCmd = Ini.GetIniString(L"",L"cmd",L"");

	HANDLE hCtrlClient = (HANDLE)Ini.GetIniUint(L"",L"target",0);
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
	CMemIniFile Ini;
	Ini.ParseMemoryDataW(pDataBuffer,dwDataLen);
	CString strCmd;
	strCmd = Ini.GetIniString(L"",L"cmd",L"");
	if (strCmd == L"getonlineclient")
	{
		CMemIniFile IniResponse;

		list<HANDLE> lstHandles;
		IOCPGetAllOnlineClient(hDebugerClientServer,OnlineClientEnumCallBack,&lstHandles);

 		IniResponse.WriteIniString(L"",L"cmd",L"getonlineclient");
 		IniResponse.WriteIniInt(L"",L"clientcount",lstHandles.size());
 
 		int nIndex = 0;
 		HANDLE hTempHandle = NULL;
 		for (list<HANDLE>::const_iterator it = lstHandles.begin();it!=lstHandles.end();it++)
 		{
 			hTempHandle = *it;
 
 			CString strTempKeyName;
 			strTempKeyName.Format(L"client%d",nIndex);
 			IniResponse.WriteIniInt(L"",strTempKeyName,(int)hTempHandle);
 			nIndex++;
 		}
 
 		CString strResponse;
 		strResponse = IniResponse.BuildData();
 
 		IOCPPostSendRequest(hControllerClientServer,hClient,(BYTE *)strResponse.GetBuffer(),strResponse.GetLength()*sizeof(WCHAR),NULL);
	}

	if (strCmd == L"runcmd")
	{
		HANDLE hTargetClient = (HANDLE)Ini.GetIniUint( L"",L"target", 0 );
		if (hTargetClient)
		{
			Ini.WriteIniInt(L"",L"source",(int)hClient);
			CString strSendData;
			strSendData = Ini.BuildData();

			IOCPPostSendRequest(hDebugerClientServer,hTargetClient,(BYTE *)strSendData.GetBuffer(),strSendData.GetLength()*sizeof(WCHAR),NULL);
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

