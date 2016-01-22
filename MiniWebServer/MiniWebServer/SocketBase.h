#pragma once

#include <WinSock2.h>
#include <ws2def.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

class CSocketBase
{
public:
	static IN_ADDR m_LocalIP;
public:
	CSocketBase(void);
	~CSocketBase(void);
};
