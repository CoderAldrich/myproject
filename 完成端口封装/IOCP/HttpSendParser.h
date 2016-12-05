#pragma once
#include <atlstr.h>
#include <list>
using namespace std;

class CHttpSendParser
{
	typedef struct REQUEST_NODE
	{
		CStringA strRequestName;
		CStringA strRequestValue;
	}REQUEST_NODE,*PREQUEST_NODE;
	typedef list<REQUEST_NODE> REQUEST_LIST;
	typedef REQUEST_LIST::iterator REQUEST_LIST_PTR;

protected:
	CStringA m_strRequestMeth;
	CStringA m_strRequestPath;
	CStringA m_strRequestVersion;
	CStringA m_strRequestHost;
	CStringA m_strRequestProtocol;

	REQUEST_LIST m_RequestList;
	LONG         m_ContentStart;
	BOOL         m_bHaveProxy;
public:
	CHttpSendParser(void);
	~CHttpSendParser(void);
	BOOL ParseData(const char * pBuffer,int nLen);
	LONG GetContentStart();
	CStringA GetParseUrl(void);
	CStringA GetRequestMeth();
	BOOL     GetHaveProxy();
	CStringA GetValueByName(CStringA strName);
	CStringA GetValueByIndex( int nIndex,CStringA &strName );
	BOOL SetValueByName(CStringA strName,CStringA strValue);
	BOOL SetPath(CStringA strNewPath);
	CStringA GetHost(void);
	BOOL BuildBuffer(CStringA &strNewBuffer);
	BOOL AddRequestNode(CStringA strName,CStringA strValue);
	BOOL DelRequestNode(CStringA strName);
	BOOL SetHttpVersion(CStringA strHttpVersion);
};
