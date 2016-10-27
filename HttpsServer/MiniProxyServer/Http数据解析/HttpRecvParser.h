#pragma once

#include <atlstr.h>
#include <list>
using namespace std;


class CHttpRecvParser
{
	typedef struct RESPONSE_NODE
	{
		CStringA strResponseName;
		CStringA strResponseValue;
	}RESPONSE_NODE,*PRESPONSE_NODE;
	typedef list<RESPONSE_NODE> RESPONSE_LIST;
	typedef RESPONSE_LIST::iterator RESPONSE_LIST_PTR;
protected:
	CStringA m_strHttpVersion;
	CStringA m_strResponseCode;
	CStringA m_strResponseText;
	RESPONSE_LIST m_ResponseList;
	BOOL m_bResponseHead;
	LONG   m_ContentStart;
public:
	CHttpRecvParser(void);
	~CHttpRecvParser(void);
	BOOL ParseData(const char * pBuffer,int nLen);
	BOOL IsResponseHead();
	LONG GetContentStart();
	CStringA GetResponseCode();
	CStringA GetValueByName(CStringA strName);
	BOOL SetValueByName(CStringA strName,CStringA strValue);
	BOOL BuildBuffer(CStringA &strNewBuffer);
	BOOL AddResponseNode(CStringA strName,CStringA strValue);
	BOOL DelResponseNode(CStringA strName);

};
