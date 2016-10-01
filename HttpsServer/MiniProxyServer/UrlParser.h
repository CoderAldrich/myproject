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
private:
	CStringA m_strUrl;
	CStringA m_strProtocol;
	CStringA m_strHost;
	CStringA m_strPath;
	ParamList m_ParamList;
public:
	CUrlParser(void);
	~CUrlParser(void);
	BOOL SetUrl(CStringA strNewUrl);
	BOOL ParseUrl(void);
	CStringA GetHost();
	CStringA GetPath();
	BOOL GetParamValueByName(CStringA strName,CStringA &strValue);
	BOOL SetParamValueByName(CStringA strName,CStringA strValue);
	BOOL AddOrSetParamValue(CStringA strName,CStringA strValue);
	CStringA BuildUrl();
	CStringA BuildPath();
};
