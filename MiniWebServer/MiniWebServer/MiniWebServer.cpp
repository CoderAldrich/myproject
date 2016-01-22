// HomePopup.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "TcpSocket.h"
#include "HttpSendParser.h"
#include "UrlParser.h"

#include <atlstr.h>

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
// 				CStringA strOpenUrl;
// 				urlparser.GetParamValueByName("url",strOpenUrl);
// 				
// 				if ( strOpenUrl.GetLength() > 0 )
// 				{
// 					strOpenUrl = CStringA(CEnDecryption::GetDecryptStr(CString(strOpenUrl)));
// 
// 					STARTUPINFO si;
// 					PROCESS_INFORMATION pi;
// 					ZeroMemory(&pi,sizeof(PROCESS_INFORMATION));
// 					ZeroMemory(&si,sizeof(STARTUPINFO));
// 					si.cb=sizeof(STARTUPINFO);
// 
// 					CString strCmdLine;
// 					strCmdLine.Format(L" cmd /c \"start %s\"",CString(strOpenUrl));
// 
// 					bResponseRes = CreateProcessW(TEXT("C:\\Windows\\System32\\cmd.exe"),strCmdLine.GetBuffer(),NULL,NULL,FALSE,CREATE_NO_WINDOW ,NULL,NULL,&si,&pi);			
// 					CloseHandle(pi.hThread);
// 					CloseHandle(pi.hProcess);
// 				}
			}
		}
	}


	LPCSTR pszResponseDataFormat=
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: %d\r\n"
		"Connection: close\r\n\r\n%s";

	char chResponseData[2000];
	sprintf_s(chResponseData,2000,pszResponseDataFormat,1,bResponseRes?"1":"0");

	sock.SendData((PVOID)chResponseData,strlen(chResponseData));

	sock.CloseTcpSocket();

	return 0;
}
// 导出函数
VOID WINAPI HPRun(LPCWSTR pszCmdLine)
{
	CTcpSocket sockListen;
	BOOL bRes = sockListen.CreateTcpSocket();
	if (bRes)
	{
		sockListen.InitAccept(54110);

		while (TRUE)
		{
			SOCKET sockclient = sockListen.Accept(NULL);
			CreateThread(NULL,0,RequestHandleThread,(PVOID)sockclient,0,NULL);
		}
	}
}

#ifdef  _WINDLL
#pragma comment(linker,"/EXPORT:HPRun=?HPRun@@YGXPB_W@Z")

BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


#else
int WINAPI wWinMain(
					__in HINSTANCE hInstance,
					__in_opt HINSTANCE hPrevInstance,
					__in_opt LPWSTR lpCmdLine,
					__in int nShowCmd
					)
{
	HPRun(L"");
	while (1)
	{
		Sleep(100);
	}
	return 0;
}
#endif