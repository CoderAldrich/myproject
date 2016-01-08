#include "stdafx.h"
#include <Shlwapi.h>
#include "AutoBrowser.h"

CElementInformation::CElementInformation()
{
	pParentElemInfo = NULL;
	m_bTextFullMatch = FALSE;
}

CElementInformation::CElementInformation(LPCTSTR pszTagName)
{
	m_strTagName = pszTagName;
	pParentElemInfo = NULL;
	m_bTextFullMatch = FALSE;
}
CElementInformation::~CElementInformation()
{
	if ( pParentElemInfo )
	{
		delete pParentElemInfo;
	}
}
BOOL CElementInformation::SetTagName(LPCTSTR pszTagName)
{
	if (wcslen(pszTagName))
	{
		m_strTagName = pszTagName;
		m_strTagName.Trim();
		return TRUE;
	}

	return FALSE;
}

BOOL CElementInformation::SetTextName(LPCTSTR pszTextName,BOOL bFullMatch)
{
	m_strElemText = pszTextName;
	m_bTextFullMatch = bFullMatch;
	m_strElemText.Trim();
	return TRUE;
}
BOOL CElementInformation::AddElementAttribute(LPCTSTR pszAttributeName,LPCTSTR pszAttributeValue,BOOL bFullMatch)
{
	ATTRIBUTE_MAP_PTR it = m_AttrMap.find(pszAttributeName);
	if (it!=m_AttrMap.end())
	{
		return FALSE;
	}

	ATTRIBUTE_VALUE_INFO Info;
	Info.strValue = pszAttributeValue;
	Info.bFullMatch = bFullMatch;
	Info.strValue.Trim();

	m_AttrMap.insert(make_pair(CString(pszAttributeName).Trim(),Info));
	return TRUE;
}
BOOL CElementInformation::DelElementAttribute(LPCTSTR pszAttributeName)
{

	ATTRIBUTE_MAP_PTR it = m_AttrMap.find(pszAttributeName);
	if (it!=m_AttrMap.end())
	{
		m_AttrMap.erase(it);
		return TRUE;
	}
	return FALSE;
}
BOOL CElementInformation::MatchElementAttribute(IHTMLElement *pElem)
{
	BOOL bMatch = FALSE;


	if (pElem)
	{
		BSTR bstrTagName;
		pElem->get_tagName(&bstrTagName);
		BSTR bstrElemText;
		pElem->get_outerText(&bstrElemText);

		CString strTagName;
		strTagName = bstrTagName;
		strTagName.Trim();
		CString strElemText;
		strElemText = bstrElemText;
		strElemText.Trim();
		
		BOOL bTextMatch = FALSE;
		if (m_strElemText.GetLength() == 0)
		{
			bTextMatch = TRUE;
		}
		else
		{
			if (m_bTextFullMatch)
			{
				bTextMatch = m_strElemText.CompareNoCase(strElemText) == 0;
			}
			else
			{
				bTextMatch = strElemText.Find(m_strElemText) >= 0;
			}
		}

		if( bTextMatch && m_strTagName.CompareNoCase(strTagName) == 0 )
		{
			BOOL bAttributeMatch = TRUE;
			for (ATTRIBUTE_MAP_PTR it = m_AttrMap.begin();it!=m_AttrMap.end();it++)
			{
				CString strAttributeName;
				CString strAttributeValue;
				BOOL bFullMatch = TRUE;
				strAttributeName = it->first;
				strAttributeValue = it->second.strValue;
				bFullMatch = it->second.bFullMatch;

//  #ifdef DEBUG
//  				OutputDebugStringW(L"Rule: "+strAttributeName+L" "+strAttributeValue+L"\n");
//  #endif
				CComVariant vtAttrValue;
				HRESULT hr =pElem->getAttribute(strAttributeName.AllocSysString(),2,&vtAttrValue);

				if (S_OK != hr || vtAttrValue.vt != VT_BSTR || CString(vtAttrValue.bstrVal).GetLength() == 0)
				{
					//如果是class属性 尝试通过get_className方式获取
					if(StrCmpIW( strAttributeName , L"class" ) == 0)
					{
						BSTR bstrClassName;
						hr = pElem->get_className(&bstrClassName);
						if (S_OK != hr || CString(bstrClassName).GetLength() == 0)
						{
							bAttributeMatch = FALSE;
							break;
						}
						vtAttrValue = bstrClassName;
					}
					else //获取失败就认为是不匹配
					{
						bAttributeMatch = FALSE;
						break;
					}

				}
			
				CString strTempAttributeValue;
				strTempAttributeValue = vtAttrValue.bstrVal;
				strTempAttributeValue.Trim();
//  #ifdef DEBUG
//  				OutputDebugStringW(L"GetValue："+strTempAttributeValue+L"\n");
//  				if (strTempAttributeValue.Find(L"www.baidu.com/baidu.php") > 0)
//  				{
//  					int a=0;
//  				}
//  #endif
				if (bFullMatch)
				{
					if (StrCmpIW( strAttributeValue , strTempAttributeValue ) != 0)
					{
						bAttributeMatch = FALSE;
						break;
					}
				}
				else
				{
					if (StrStrIW( strTempAttributeValue  , strAttributeValue  ) == NULL)
					{
						bAttributeMatch = FALSE;
						break;
					}
				}
			}

			bMatch = bAttributeMatch;

		}

		//在检查父节点
		if (bMatch && pParentElemInfo)
		{
			//bMatch = FALSE;
			CComQIPtr<IHTMLElement> pqParentElem;
			pElem->get_parentElement(&pqParentElem);
			BSTR classname;
			pqParentElem->get_className(&classname);

			BSTR tagname;
			pqParentElem->get_tagName(&tagname);

			if (pqParentElem)
			{
				bMatch = pParentElemInfo->MatchElementAttribute(pqParentElem);
			}
		}

	}
	return bMatch;
}


CElementInformation *CElementInformation::CreateParentInfo()
{
	if( pParentElemInfo == NULL )
	{
		pParentElemInfo = new CElementInformation;
	}
	return pParentElemInfo;
}