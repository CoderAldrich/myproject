// RemoteDebuger.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "RemoteDebuger.h"
#include "TcpSocket.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	CTcpSocket tcpSock;
	tcpSock.CreateTcpSocket();
	BOOL bConRes = tcpSock.Connect( "gz8912.jios.org",8081 );


	return (int) 0;
}