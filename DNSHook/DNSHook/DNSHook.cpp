// DNSHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <atlstr.h>
struct DNSHeader
{
	USHORT usTransID; //标识符
	USHORT usFlags; //各种标志位
	USHORT usQuestionCount; //Question字段个数 
	USHORT usAnswerCount; //Answer字段个数
	USHORT usAuthorityCount; //Authority字段个数
	USHORT usAdditionalCount; //Additional字段个数
};

#define MAX_DOMAINNAME_LEN  255
#define DNS_PORT            53
#define DNS_TYPE_SIZE       2
#define DNS_CLASS_SIZE      2
#define DNS_TTL_SIZE        4
#define DNS_DATALEN_SIZE    2
#define DNS_TYPE_A          0x0001 //1 a host address
#define DNS_TYPE_CNAME      0x0005 //5 the canonical name for an alias
#define DNS_PACKET_MAX_SIZE (sizeof(DNSHeader) + MAX_DOMAINNAME_LEN + DNS_TYPE_SIZE + DNS_CLASS_SIZE)


BOOL DecodeDotStr(char *szEncodedStr, USHORT *pusEncodedStrLen, char *szDotStr, USHORT nDotStrSize,char *szPacketStartPos = NULL)
{
	if (szEncodedStr == NULL || pusEncodedStrLen == NULL || szDotStr == NULL)
	{
		return FALSE;
	}

	char *pDecodePos = szEncodedStr;
	USHORT usPlainStrLen = 0;
	BYTE nLabelDataLen = 0;    
	*pusEncodedStrLen = 0;

	while ((nLabelDataLen = *pDecodePos) != 0x00)
	{
		if ((nLabelDataLen & 0xc0) == 0) //普通格式，LabelDataLen + Label
		{
			if (usPlainStrLen + nLabelDataLen + 1 > nDotStrSize)
			{
				return FALSE;
			}
			memcpy(szDotStr + usPlainStrLen, pDecodePos + 1, nLabelDataLen);
			memcpy(szDotStr + usPlainStrLen + nLabelDataLen, ".", 1);
			pDecodePos += (nLabelDataLen + 1);
			usPlainStrLen += (nLabelDataLen + 1);
			*pusEncodedStrLen += (nLabelDataLen + 1);
		}
		else //消息压缩格式，11000000 00000000，两个字节，前2位为跳转标志，后14位为跳转的偏移
		{
			if (szPacketStartPos == NULL)
			{
				return FALSE;
			}
			USHORT usJumpPos = ntohs(*(USHORT*)(pDecodePos)) & 0x3fff;
			USHORT nEncodeStrLen = 0;
			if (!DecodeDotStr(szPacketStartPos + usJumpPos, &nEncodeStrLen, szDotStr + usPlainStrLen, nDotStrSize - usPlainStrLen, szPacketStartPos))
			{
				return FALSE;
			}
			else
			{
				*pusEncodedStrLen += 2;
				return TRUE;
			}
		}
	}

	szDotStr[usPlainStrLen - 1] = '\0';
	*pusEncodedStrLen += 1;

	return TRUE;
}

BOOL DecodeDNSResponse( char *pRecvBuf,int nBufLen )
{
	USHORT nEncodedNameLen = 0;
	char szDotName[128] = {'\0'};
	DNSHeader *pDNSHeader = (DNSHeader*)pRecvBuf;
	USHORT usQuestionCount = 0;
	USHORT usAnswerCount = 0;

	if ( 
		//pDNSHeader->usTransID == 0
		//&& 
		(ntohs(pDNSHeader->usFlags) & 0xfb7f) == 0x8100 //RFC1035 4.1.1(Header section format)
		&& (usQuestionCount = ntohs(pDNSHeader->usQuestionCount)) >= 0
		&& (usAnswerCount = ntohs(pDNSHeader->usAnswerCount)) > 0)
	{
		CStringA strHostName;

		char *pDNSData = pRecvBuf + sizeof(DNSHeader);

		//解析Question字段
		for (int q = 0; q != usQuestionCount; ++q)
		{
			if (!DecodeDotStr(pDNSData, &nEncodedNameLen, szDotName, sizeof(szDotName)))
			{
				return FALSE;
			}
			pDNSData += (nEncodedNameLen + DNS_TYPE_SIZE + DNS_CLASS_SIZE);
		}

		strHostName = szDotName;

		OutputDebugStringA(szDotName);

		//解析Answer字段
		for (int a = 0; a != usAnswerCount; ++a)
		{
			if (!DecodeDotStr(pDNSData, &nEncodedNameLen, szDotName, sizeof(szDotName), pRecvBuf))
			{
				return FALSE;
			}

			pDNSData += nEncodedNameLen;

			USHORT usAnswerType = ntohs(*(USHORT*)(pDNSData));
			USHORT usAnswerClass = ntohs(*(USHORT*)(pDNSData + DNS_TYPE_SIZE));
			ULONG usAnswerTTL = ntohl(*(ULONG*)(pDNSData + DNS_TYPE_SIZE + DNS_CLASS_SIZE));
			USHORT usAnswerDataLen = ntohs(*(USHORT*)(pDNSData + DNS_TYPE_SIZE + DNS_CLASS_SIZE + DNS_TTL_SIZE));
			pDNSData += (DNS_TYPE_SIZE + DNS_CLASS_SIZE + DNS_TTL_SIZE + DNS_DATALEN_SIZE);

			if (usAnswerType == DNS_TYPE_A /*&& pveculIPList != NULL*/)
			{
				if ( strHostName.Find("9108.com") >= 0 )
				{
					ULONG ulIP = *(ULONG*)(pDNSData);
					in_addr ia;
					ia.s_addr=ulIP;
					OutputDebugStringA(inet_ntoa(ia));

					*(ULONG*)(pDNSData) = inet_addr("127.0.0.1");
				}
			}
// 			else if (usAnswerType == DNS_TYPE_CNAME && pvecstrCNameList != NULL)
// 			{
// 				if (!DecodeDotStr(pDNSData, &nEncodedNameLen, szDotName, sizeof(szDotName), recvbuf))
// 				{
// 					return FALSE;
// 				}
// 				pvecstrCNameList->push_back(szDotName);
// 			}

			pDNSData += (usAnswerDataLen);
		}

	}

	return FALSE;
}



