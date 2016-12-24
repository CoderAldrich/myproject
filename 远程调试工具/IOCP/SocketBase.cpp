#include "StdAfx.h"
#include "SocketBase.h"

IN_ADDR CSocketBase::m_LocalIP;
DWORD CSocketBase::m_dwTlsIndex = 0;
CSocketBase::CSocketBase(void)
{
	BOOL bFirst = FALSE;
	static BOOL bInitStartup = FALSE;
	if ( FALSE == bInitStartup )
	{
		bInitStartup = TRUE;
		m_dwTlsIndex = TlsAlloc();
		bFirst = TRUE;
	}

	PVOID pWSAStart = TlsGetValue(m_dwTlsIndex);
	if ( NULL == pWSAStart )
	{
		WSADATA wsd;
		if( WSAStartup( MAKEWORD(2,2),&wsd) != 0 )
		{

		}

		TlsSetValue(m_dwTlsIndex,(PVOID)1);
	}
	
	if (bFirst)
	{
		char hostname[256];  
		if (gethostname(hostname,sizeof(hostname))!=SOCKET_ERROR)  
		{  
			HOSTENT* host=NULL;  
			if ((host = gethostbyname(hostname))!=NULL)  
			{
				in_addr *pia = (in_addr*)*host->h_addr_list;
				memcpy(&m_LocalIP,pia,sizeof(in_addr));
			}
		}
	}
}

CSocketBase::~CSocketBase(void)
{
}
