#include "StdAfx.h"
#include "SocketBase.h"

IN_ADDR CSocketBase::m_LocalIP;

CSocketBase::CSocketBase(void)
{
	static BOOL bInitStartup = FALSE;
	if ( FALSE == bInitStartup )
	{
		bInitStartup = TRUE;

		WSADATA wsd;
		if( WSAStartup( MAKEWORD(2,2),&wsd) != 0 )
		{

		}

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
