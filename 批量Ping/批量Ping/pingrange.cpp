


// MyPing.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <atlstr.h>

#include <map>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

typedef struct PingNode
{
	CString strIP;
	DWORD   dwIPHost;
	DWORD   dwIPNet;
	DWORD   dwSendTime;
	BOOL    bRespon;
}PingNode,*PPingNode;

typedef map<USHORT,PingNode> PingMap;
typedef PingMap::iterator    PingMapPtr;



#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0
#define ICMP_MIN 8 //Minimum 8-byte ICMP packet (header)

#define DEF_PACKET_SIZE 32
#define MAX_PACKET 1024
#define MAX_IP_HDR_SIZE 60

//IP header structure
typedef struct _iphdr
{
	unsigned int h_len:4;//Length of the header
	unsigned int version:4;//Version of IP
	unsigned char tos;//Type of service
	unsigned short total_len;//Total length of the packet
	unsigned short ident;//Unique identifier
	unsigned short frag_and_flags;//Flags
	unsigned char ttl;//Time to live
	unsigned char proto;//Protocol (TCP,UDP,etc.)
	unsigned short checksum;//IP checksum

	unsigned int sourceIP;
	unsigned int destIP;
} IpHeader;

//ICMP header structure
typedef struct _icmphdr
{
	BYTE i_type;
	BYTE i_code;//Type sub code
	USHORT i_cksum;
	USHORT i_id;
	USHORT i_seq;

	//This is not the standard header, but we reserve space for time
	ULONG timestamp;
} IcmpHeader;

//IP option header--use with socket option IP_OPTIONS
typedef struct _ipoptionhdr
{
	unsigned char code;//Option type
	unsigned char len;//Length of option hdr
	unsigned char ptr;//Offset into optons
	unsigned long addr[9];//List of IP addrs
} IpOptionHeader;



//Helper function to fill in various fields for our ICMP request
void FillICMPData(char* icmp_data, int datasize,USHORT pkgID)
{
	IcmpHeader* icmp_hdr = (IcmpHeader*)icmp_data;
	icmp_hdr->i_type = ICMP_ECHO;//Request an ICMP echo
	icmp_hdr->i_code = 0;
	icmp_hdr->i_id = (USHORT)pkgID;
	icmp_hdr->i_cksum = 0;
	icmp_hdr->i_seq = 0;

	char* datapart = icmp_data + sizeof(IcmpHeader);

	//Place some junk in the buffer
	memset(datapart, 'E', datasize - sizeof(IcmpHeader));
}

//This function calculates the 16-bit one's complement sum
//of the supplied buffer (ICMP) header
USHORT CheckSum(USHORT* pBuffer, int nSize)
{
	unsigned long cksum = 0;

	while (nSize > 1)
	{
		cksum += *pBuffer++;
		nSize -= sizeof(USHORT);
	}

	if (nSize)
	{
		cksum += *(UCHAR*)pBuffer;
	}

	cksum = (cksum>>16) + (cksum & 0xffff);
	cksum += (cksum>>16);

	return (USHORT)(~cksum);
}

//If the IP option header is present, find the IP options
//within the IP header and print the record route option values
void DecodeIPOptions(char* buf, int bytes)
 {
 	IpOptionHeader* ipopt = (IpOptionHeader*)(buf + 20);
 
 	printf("RR:    ");
 	for (int i = 0; i < (ipopt->ptr / 4) - 1; i++)
 	{
 		IN_ADDR inaddr;
 		inaddr.S_un.S_addr = ipopt->addr[i];
 
 		if (i != 0)
 		{
 			printf("  ");
 		}
 
 		HOSTENT* host = gethostbyaddr((char*)&inaddr.S_un.S_addr,
 			sizeof(inaddr.S_un.S_addr), AF_INET);
 		if (host)
 		{
 			printf("(%-15s) %s\n", inet_ntoa(inaddr), host->h_name);
 		}
 		else
 		{
 			printf("(%-15s)\n", inet_ntoa(inaddr));
 		}
 	}
 
 	return;
 }

//The response is an IP packet. We must decode the IP header to
//locate the ICMP data.
USHORT DecodeICMPHeader(char* buf, int bytes, struct sockaddr_in* from)
{
	IpHeader* iphdr = (IpHeader*)buf;

	//Number of 32-bit words * 4 = bytes
	unsigned short iphdrlen = iphdr->h_len * 4;
	DWORD tick = GetTickCount();


	if (bytes < iphdrlen + ICMP_MIN)
	{
		char a[100];
		printf(a,"Too few bytes from %s\n", inet_ntoa(from->sin_addr));
		//OutputDebugStringA(a);
	}

	IcmpHeader* icmphdr = (IcmpHeader*)(buf + iphdrlen);
	if (icmphdr->i_type != ICMP_ECHOREPLY)
	{
		char a[100];
		sprintf(a,"nonecho type %d recvd\n", icmphdr->i_type);
		//OutputDebugStringA(a);
		return 0;
	}

	return icmphdr->i_id;

}

DWORD PingRange(const char *pchStartIP,const char *pchEndIP,PingMap *pMap,UINT nPingTimes,UINT msTimeSpace)
{
	DWORD dwErrorCode = ERROR_SUCCESS;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		dwErrorCode = 1;
		goto end;
	}

	
	SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sockRaw == INVALID_SOCKET)
	{
		dwErrorCode = 2;
		goto end;
	}

	//Set the send/recv timeout values
	struct sockaddr_in from;
	int fromlen = sizeof(from);

	int timeout = 500;
	int setres = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO,
		(char*)&timeout, sizeof(timeout));


	ULONG ulIPStart = ntohl(inet_addr(pchStartIP));
	ULONG ulIPEnd = ntohl(inet_addr(pchEndIP));

// 	for (DWORD ulIP = ulIPStart;ulIP < ulIPEnd;ulIP++)
// 	{
// 		PingNode Node;
// 		Node.bRespon = FALSE;
// 		
// 		pMap->insert(make_pair(ulIP,Node));
// 	}
	

	//Create the ICMP packet
	int datasize = DEF_PACKET_SIZE;
	datasize += sizeof(IcmpHeader);

	char* icmp_data = (char*)HeapAlloc(GetProcessHeap(),
		HEAP_ZERO_MEMORY, MAX_PACKET);
	if (!icmp_data)
	{
		dwErrorCode = 6;
		goto end;
	}

	USHORT seq_no = 0;
	dwErrorCode = 10;
	for(int i=0;i<nPingTimes;i++)
	{
		CString strMsgOut;
		strMsgOut.Format(L"第%d次扫描..\r\n",i+1);
		OutputDebugStringW(strMsgOut);

		for (DWORD ulIP = ulIPStart;ulIP < ulIPEnd;ulIP++)
		{
			PingMapPtr it = (*pMap).find((USHORT)ulIP);

			if ( it != pMap->end() )
			{
				if( it->second.bRespon )
				{
					continue;
				}
			}
			else
			{
				in_addr ia;
				ia.s_addr = htonl(ulIP);
				PingNode Node;
				Node.bRespon = FALSE;
				Node.dwIPHost = ulIP;
				Node.dwIPNet = 0;
				Node.strIP = inet_ntoa(ia);
				Node.dwSendTime = GetTickCount();

				pMap->insert(make_pair((USHORT)ulIP,Node));

				it = (*pMap).find((USHORT)ulIP);
			}
			
			it->second.dwSendTime = GetTickCount();

			struct sockaddr_in dest;
			memset(&dest, 0, sizeof(dest));
			dest.sin_family = AF_INET;
			dest.sin_addr.s_addr = htonl(ulIP);

			memset(icmp_data, 0, MAX_PACKET);
			FillICMPData(icmp_data, datasize,ulIP);

			((IcmpHeader*)icmp_data)->i_cksum = 0;
			((IcmpHeader*)icmp_data)->timestamp = GetTickCount();
			((IcmpHeader*)icmp_data)->i_seq = i+1;
			((IcmpHeader*)icmp_data)->i_cksum = CheckSum((USHORT*)icmp_data, datasize);

			int bwrote = sendto(sockRaw, icmp_data, datasize, 0,
				(struct sockaddr*)&dest, sizeof(dest));

			if (bwrote == SOCKET_ERROR)
			{
				if ( WSAGetLastError() == WSAETIMEDOUT )
				{
					continue;
				}
				dwErrorCode = 8;
				goto end;
			}
			Sleep(msTimeSpace);
		}
	}

end:
	if (sockRaw != INVALID_SOCKET)
	{
		closesocket(sockRaw);
	}
	if( icmp_data )
	{
		HeapFree(GetProcessHeap(), 0, icmp_data);
	}

	WSACleanup();

	return dwErrorCode;
}

#include "TcpSocket.h"


//临界区互斥锁
class CCSLock
{
private:
	CRITICAL_SECTION m_cs;
public:
	CCSLock()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CCSLock()
	{
		DeleteCriticalSection(&m_cs);
	}
	VOID Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	VOID UnLock()
	{
		LeaveCriticalSection(&m_cs);
	}

};


CCSLock csLock;
#include <list>
using namespace std;
typedef list<CStringA> LIST_CHECK_HOSTS;
typedef LIST_CHECK_HOSTS::iterator LIST_CHECK_HOSTS_PTR;

LIST_CHECK_HOSTS lstHosts;


DWORD WINAPI CheckPortThread(PVOID pParam)
{

	while(1)
	{
		CStringA strHostIp;

		csLock.Lock();
		if ( lstHosts.size() > 0 )
		{
			LIST_CHECK_HOSTS_PTR it = lstHosts.begin();
			strHostIp = *it;
			lstHosts.erase(it);
		}

		csLock.UnLock();

		if ( strHostIp.GetLength() > 0 )
		{
			CTcpSocket tcpSock;
			BOOL bRes = tcpSock.CreateTcpSocket();

			bRes = tcpSock.Connect(strHostIp,80,1);
			if (bRes)
			{
				OutputDebugStringA(strHostIp+"\r\n");
			}
// 			else
// 			{
// 				OutputDebugStringA(strHostIp+" 80 no\r\n");
// 			}
			tcpSock.CloseTcpSocket();
			int a=0;

		}
		else
		{
			Sleep(10);
		}

	}


	return 0;
}

DWORD WINAPI WatchEchoPackge(LPVOID pParam)
{
	PingMap *pMap = (PingMap *)pParam;

	DWORD dwErrorCode = ERROR_SUCCESS;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		dwErrorCode = 1;
		goto end;
	}


	SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP,
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sockRaw == INVALID_SOCKET)
	{
		dwErrorCode = 2;
		goto end;
	}

	//Set the send/recv timeout values
	struct sockaddr_in from;
	int fromlen = sizeof(from);
	int timeout = 1000;

	int bread = setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO,
		(char*)&timeout, sizeof(timeout));
	if (bread == SOCKET_ERROR)
	{
		dwErrorCode = 3;
		goto end;
	}

	timeout = 1000;
	bread = setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO,
		(char*)&timeout, sizeof(timeout));
	if (bread == SOCKET_ERROR)
	{
		dwErrorCode = 4;
		goto end;
	}

	char* pRecvBuf = (char*)HeapAlloc(GetProcessHeap(),
		HEAP_ZERO_MEMORY, MAX_PACKET);

	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr("127.0.0.1");
	sendto(sockRaw, NULL, 0, 0,
		(struct sockaddr*)&dest, sizeof(dest));

	dwErrorCode = 10;

	while(true)
	{

		int bread = recvfrom(sockRaw, pRecvBuf, MAX_PACKET, 0,
			(struct sockaddr*)&from, &fromlen);
		if (bread == SOCKET_ERROR)
		{
			if ( WSAGetLastError() == WSAETIMEDOUT )
			{
				continue;
			}
			DWORD dwErrorCode = WSAGetLastError();
			dwErrorCode = 9;
			goto end;
		}
		USHORT nPkgID = DecodeICMPHeader(pRecvBuf, bread, &from);

		if (nPkgID != 0)
		{
			PingMapPtr it = pMap->find(nPkgID);
			if (it!=pMap->end())
			{
				(*pMap)[nPkgID].bRespon = true;

//  				static int nIndex = 1;
//  				CString msgout;
//  				msgout.Format(TEXT(" %05d %s %dms\r\n"),nIndex++,(*pMap)[nPkgID].strIP,GetTickCount()-it->second.dwSendTime);
//  				OutputDebugStringW(msgout);

 				csLock.Lock();
 				lstHosts.push_back(CStringA((*pMap)[nPkgID].strIP));
 				csLock.UnLock();

			}
			


//  			ULONG MacAddr[2];
//  			ULONG PhysAddrLen = 6;
//  			memset(&MacAddr, 0xff, sizeof (MacAddr));
//  
//  			int TryTimes = 0;
//  			DWORD res = 0;
//  			while( (res = SendARP( DestIP,HostIP, &MacAddr, &PhysAddrLen)) != NO_ERROR)
//  			{
//  				TryTimes++;
//  				if (TryTimes >= 3)
//  				{
//  					break;
//  				}
//  			}
//  
//  			strMacAddr = GetMacString((LPVOID *)MacAddr,6);

		}
	}

end:
	if (sockRaw != INVALID_SOCKET)
	{
		closesocket(sockRaw);
	}

	if(pRecvBuf)
	{
		HeapFree(GetProcessHeap(), 0, pRecvBuf);
	}
	WSACleanup();
	return dwErrorCode;

}



int main(char *argv,int argc)
{
	PingMap pMap;
	
	
	CreateThread(NULL,0,CheckPortThread,((PVOID)&pMap),0,NULL);
	CreateThread(NULL,0,WatchEchoPackge,((PVOID)&pMap),0,NULL);
	//PingRange("192.168.0.1","192.168.0.255",&pMap,10,1);
	//PingRange("211.64.0.0","211.71.255.255",&pMap,10,5);
	PingRange("112.121.100.0","112.121.164.255",&pMap,10,10);
	
	getchar();

	return 0;
}