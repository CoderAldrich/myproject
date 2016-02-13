#pragma once
#include <atlstr.h>
#include <list>
using namespace std;
typedef struct ParamNode
{
	CStringA strParamName;
	CStringA strParamValue;
}ParamNode ;
typedef list<ParamNode> ParamList;
typedef ParamList::iterator ParamListPtr;

class CUrlParser
{
protected:
	CStringA m_strUrl;
	CStringA m_strProtocol;
	CStringA m_strHost;
	CStringA m_strPath;
	CStringA m_strFileName;
	ParamList m_ParamList;
public:
	CUrlParser(void);
	~CUrlParser(void);
	BOOL ParseUrl(LPCSTR pchUrl);
	BOOL GetParamValueByName(CStringA strName,CStringA &strValue);
	BOOL SetParamValueByName(CStringA strName,CStringA strValue);
	BOOL AddOrSetParamValue(CStringA strName,CStringA strValue);
	CStringA GetProtocol();
	CStringA GetDomain();
	CStringA GetPath();
	CStringA GetFileName();
	CStringA BuildUrl();
	CStringA BuildPath();
};
