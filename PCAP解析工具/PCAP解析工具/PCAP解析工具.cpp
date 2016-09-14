// PCAP解析工具.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <stdio.h>
#include "pcap.h"

// #include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#include <Windows.h>

void prinfPcapFileHeader(pcap_file_header *pfh){
	if (pfh==NULL) {
		return;
	}
	printf("=====================\n"
		   "magic:0x%0x\n"
		   "version_major:%u\n"
		   "version_minor:%u\n"
		   "thiszone:%d\n"
		   "sigfigs:%u\n"
		   "snaplen:%u\n"
		   "linktype:%u\n"
		   "=====================\n",
		   pfh->magic,
		   pfh->version_major,
		   pfh->version_minor,
		   pfh->thiszone,
		   pfh->sigfigs,
		   pfh->snaplen,
		   pfh->linktype);
}
 
void printfPcapHeader(pcap_header *ph){
	if (ph==NULL) {
		return;
	}
	printf("=====================\n"
		   "ts.timestamp_s:%u\n"
		   "ts.timestamp_ms:%u\n"
		   "capture_len:%u\n"
		   "len:%d\n"
		   "=====================\n",
		   ph->ts.timestamp_s,
		   ph->ts.timestamp_ms,
		   ph->capture_len,
		   ph->len);
 
 
}
 
void printPcap(void * data,size_t size){
	unsigned  short iPos = 0;
	//int * p = (int *)data;
	//unsigned short* p = (unsigned short *)data;
	if (data==NULL) {
		return;
	}
 
	printf("\n==data:0x%x,len:%lu=========",data,size);
 
	for (iPos=0; iPos < size/sizeof(unsigned short); iPos++) {
		//printf(" %x ",(int)( * (p+iPos) ));
		//unsigned short a = ntohs(p[iPos]);
 
		unsigned short a = ntohs( *((unsigned short *)data + iPos ) );
		if (iPos%8==0) printf("\n");
		if (iPos%4==0) printf(" ");
 
		printf("%04x",a);
 
 
	}
	/*
	 for (iPos=0; iPos <= size/sizeof(int); iPos++) {
		//printf(" %x ",(int)( * (p+iPos) ));
		int a = ntohl(p[iPos]);
 
		//int a = ntohl( *((int *)data + iPos ) );
		if (iPos %4==0) printf("\n");
 
		printf("%08x ",a);
 
 
	}
	 */
	printf("\n============\n");
}



#define MAX_ETH_FRAME 1518
#define ERROR_FILE_OPEN_FAILED -1
#define ERROR_MEM_ALLOC_FAILED -2
#define ERROR_PCAP_PARSE_FAILED -3

#include "B2Winet/B2WINET.H"
#include "HttpSendParser.h"

PVOID  CheckData(HANDLE hSaveFile, UCHAR* pPacketBuf ,ULONG nPacketSize,LONGLONG nPackIndex)
{
	PETHER_HEADER		pEtherHeader = NULL;
	PIP_HEADER			pIPHeader = NULL;
	PTCP_HEADER			pTcpHeader = NULL;
	ULONG nIPHeaderLen = 0;
	char *pContent = NULL;
	ULONG        i = 0;

	if(pPacketBuf)
	{
		//如果包取出来了，而且包大小，大于以太网头
		if(nPacketSize > sizeof(ETHER_HEADER) )
		{
			pEtherHeader = (PETHER_HEADER)pPacketBuf;
			DWORD dwType = ntohs(pEtherHeader->ether_type);
			
			if (dwType != 0x8100)
			{
				int a=0;
			}
			if(ntohs(pEtherHeader->ether_type) == ETHERTYPE_IP &&
				nPacketSize >= (sizeof(ETHER_HEADER) + sizeof(IP_HEADER))
				)
			{
				pIPHeader = (PIP_HEADER)(pPacketBuf + sizeof(ETHER_HEADER));
				if(pIPHeader->ip_v == 4)
				{ 

					nIPHeaderLen = pIPHeader->ip_hl * 4;
					if(pIPHeader->ip_p == IPPROTO_TCP) ////如果是TCP协议
					{

						pTcpHeader = (PTCP_HEADER)(pPacketBuf + sizeof(ETHER_HEADER) + nIPHeaderLen);
						//not fragment
						if((pIPHeader->ip_off & (~IP_OFFMASK)) == 0 ||
							(pIPHeader->ip_off & IP_DF))
						{
							ULONG nSrcPort = ntohs(pTcpHeader->th_sport);
							ULONG nDstPort = ntohs(pTcpHeader->th_dport);

							//if(nDstPort == 80)
							{
								pContent = (CHAR*)pTcpHeader + sizeof(TCP_HEADER);
								if((pContent - (char *)pPacketBuf) < htons(pIPHeader->ip_len))//(long)nPacketSize
								{
									if (
										pContent[0] == 'G' 
										&& pContent[1] == 'E'
										&& pContent[2] == 'T'
										)
									{
										CHttpSendParser parser;
										BOOL bRes = parser.ParseData(pContent,nPacketSize - sizeof(ETHER_HEADER) - sizeof(IP_HEADER)-sizeof(TCP_HEADER));
										if (bRes)
										{
											CStringA strPath;
											CStringA strHost;
											
											strPath = parser.GetPath();
											strHost = parser.GetHost();

											int a=0;
											if ( (
												strPath == "/" 
												|| 
												(
												//strPath.Left(2) == "/?" 
												//&& strPath.GetLength() < 30 
												strPath.Find("/",1) < 0
												)
												)
												&&
												(
												strPath.Find("api") < 0 &&
												strHost.Find("api") < 0 &&

												strPath.Find("image") < 0 &&
												strHost.Find("image") < 0 &&

												strPath.Find("img") < 0 &&
												strHost.Find("img") < 0 
												)
												&&
												(
												strHost.Find(".com") > 0
												|| strHost.Find(".net") > 0
												|| strHost.Find(".org") > 0
												|| strHost.Find(".com.cn") > 0
												|| strHost.Find(".net.cn") > 0
												|| strHost.Find(".org.cn") > 0
												|| strHost.Find(".gov") > 0
												)
												)
											{
												char *pchSrcIp = inet_ntoa(pIPHeader->ip_src);

												DWORD dwWriteLen = 0;
												CStringA strSrcIp;
												CStringA strParseUrl;
												CStringA strUserAgent;

												strSrcIp = pchSrcIp;
												strSrcIp+=",";

												strParseUrl = parser.GetParseUrl();
												strParseUrl="\"" +strParseUrl+ "\",";

												strUserAgent = parser.GetValueByName("User-Agent");
												strUserAgent="\"" +strUserAgent+ "\"\r\n";


												WriteFile(hSaveFile,strSrcIp.GetBuffer(),strSrcIp.GetLength(),&dwWriteLen,0);
												WriteFile(hSaveFile,strParseUrl.GetBuffer(),strParseUrl.GetLength(),&dwWriteLen,0);
												WriteFile(hSaveFile,strUserAgent.GetBuffer(),strUserAgent.GetLength(),&dwWriteLen,0);
											}

										}
										else
										{
											int a=0;
// 											CHttpSendParser parser1;
// 											BOOL bRes = parser1.ParseData(pContent,nPacketSize - sizeof(ETHER_HEADER) - sizeof(IP_HEADER)-sizeof(TCP_HEADER));

										}
									}
									
									//DbgPrint("%s\r\n",pContent);
								}
							}
						}
					}
				}
			}
		}
	}

	return NULL;
}


VOID ParsePCapFile( LPCSTR pszSrcFile )
{
	pcap_file_header  pfh;
	pcap_header  ph;
	int count=0;
	void * buff = NULL;
	int readSize=0;
	int ret = 0;

	CStringA strFileHeader;
	DWORD dwWriteLen = 0;

	CStringA strSrcFile;
	strSrcFile=pszSrcFile;
	HANDLE hFile = CreateFileA(strSrcFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	HANDLE hOutFile = CreateFileA(strSrcFile+".csv",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);


	if ( INVALID_HANDLE_VALUE == hFile  ) {
		goto MY_ERROR;
	}

	if ( INVALID_HANDLE_VALUE == hOutFile  ) {
		goto MY_ERROR;
	}


	strFileHeader = "src_ip,request_url,user_agent\r\n";

	WriteFile(hOutFile,strFileHeader.GetBuffer(),strFileHeader.GetLength(),&dwWriteLen,0);

	DWORD dwReadLen = 0;
	ReadFile(hFile,&pfh,sizeof(pcap_file_header),&dwReadLen,NULL);
	prinfPcapFileHeader(&pfh);

	buff = (void *)malloc(MAX_ETH_FRAME);
	for (count=1; ; count++) {
		memset(buff,0,MAX_ETH_FRAME);
		//read pcap header to get a packet
		//get only a pcap head count .

		ReadFile(hFile,&ph, sizeof(pcap_header),&dwReadLen,NULL);
		readSize = dwReadLen;

		if (readSize<=0) 
		{
			break;
		}

		//printfPcapHeader(&ph);

		if (buff==NULL) {
			fprintf(stderr, "malloc memory failed.\n");
			ret = ERROR_MEM_ALLOC_FAILED;
			goto MY_ERROR;
		}

		DWORD dwTotalReadLen = 0;
		while ( dwTotalReadLen < ph.capture_len )
		{
			ReadFile(hFile,(char *)buff+dwTotalReadLen, ph.capture_len-dwTotalReadLen,&dwReadLen,NULL);
			dwTotalReadLen+=dwReadLen;			
			if (dwReadLen == 0)
			{
				break;
			}
		}
		readSize = dwTotalReadLen;
		if (readSize != ph.capture_len) {
			fprintf(stderr, "pcap file parse error.\n");
			ret = ERROR_PCAP_PARSE_FAILED;
			goto MY_ERROR;
		}

		//printPcap(buff, ph.capture_len);

		int nOffset = 4;
		CheckData(hOutFile,(UCHAR *)buff+nOffset,ph.capture_len-nOffset,0);

		//printf("===count:%d,readSize:%d===\n",count,readSize);
		if (readSize == 0 )
		{
			break;
		}
	}

MY_ERROR:
	//free
	if (buff) {
		free(buff);
		buff=NULL;
	} 
	CloseHandle(hFile);
	CloseHandle(hOutFile);
}

int main (int argc, const char * argv[])
{
	if ( argc == 2 )
	{
		ParsePCapFile(argv[1]);
	}
	//for (int i=9;i<=12;i++)
	//{
	//	CStringA strSrcFile;
	//	strSrcFile.Format("C:\\Users\\GAOZAN\\Documents\\Tencent Files\\2592705588\\FileRecv\\pcapdata\\t%d.pcap",i);

		

	//}

	return 0;
}
