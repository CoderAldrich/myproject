#pragma once
#include <map>
using namespace std;
typedef struct TAG_ATTRIBUTE_VALUE_INFO
{
	CString strValue;
	BOOL    bFullMatch;
}ATTRIBUTE_VALUE_INFO,PATTRIBUTE_VALUE_INFO;

typedef map<CString,ATTRIBUTE_VALUE_INFO> ATTRIBUTE_MAP;
typedef ATTRIBUTE_MAP::iterator ATTRIBUTE_MAP_PTR;
class CElementInformation
{
public:
	CElementInformation(LPCTSTR pszTagName);
	CElementInformation();
	~CElementInformation();
protected:
	CString m_strTagName;
	CString m_strElemText;
	BOOL    m_bTextFullMatch;
	ATTRIBUTE_MAP m_AttrMap;
	CElementInformation *pParentElemInfo;
public:
	BOOL SetTagName(LPCTSTR pszTagName);
	BOOL SetTextName(LPCTSTR pszTextName,BOOL bFullMatch);
	BOOL AddElementAttribute(LPCTSTR pszAttributeName,LPCTSTR pszAttributeValue,BOOL bFullMatch);
	BOOL DelElementAttribute(LPCTSTR pszAttributeName);
	BOOL MatchElementAttribute(IHTMLElement *pElem);
	
	CElementInformation *CreateParentInfo();

};