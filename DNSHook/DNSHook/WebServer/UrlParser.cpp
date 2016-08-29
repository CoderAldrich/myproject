#include "StdAfx.h"
#include "HelpFun.h"
#include "UrlParser.h"

CUrlParser::CUrlParser(void)
{

}

CUrlParser::~CUrlParser(void)
{
}

BOOL CUrlParser::SetUrl(CStringA strNewUrl)
{
	m_strUrl = strNewUrl;
	return TRUE;
}

BOOL CUrlParser::ParseUrl(void)
{
	CStringA strTempStrs[2];
	DivisionString("?",m_strUrl,strTempStrs,2);
	
	CStringA strTempStrs1[2];
	DivisionString("://",strTempStrs[0],strTempStrs1,2);
	m_strProtocol = strTempStrs1[0];

	int nStart = strTempStrs1[1].Find("/");
	if (nStart > 0)
	{
		m_strHost = strTempStrs1[1].Left(nStart);
		m_strPath = strTempStrs1[1].Right(strTempStrs1[1].GetLength() - nStart);
	}
	CStringA strTempStrs3[30];
	int nParamCount = DivisionString("&",strTempStrs[1],strTempStrs3,30);
	for (int i=0;i<nParamCount;i++)
	{
		CStringA strTempStrs4[2];
		DivisionString("=",strTempStrs3[i],strTempStrs4,2);

		AddOrSetParamValue(strTempStrs4[0],strTempStrs4[1]);

	}
	return TRUE;
}

BOOL CUrlParser::GetParamValueByName(CStringA strName,CStringA &strValue)
{
	BOOL bFound = FALSE;
	
	if(strName.GetLength() == 0)
	{
		return bFound;
	}

	strName.MakeLower();

	for (ParamListPtr it = m_ParamList.begin();it!=m_ParamList.end();it++)
	{
		if ( CStringA(it->strParamName).MakeLower() == strName )
		{
			strValue = it->strParamValue;
			bFound = TRUE;
			break;
		}
	}
	
	return bFound;
}
BOOL CUrlParser::SetParamValueByName(CStringA strName,CStringA strValue)
{
	BOOL bRes = FALSE;
	if(strName.GetLength() == 0)
	{
		return bRes;
	}
	strName.MakeLower();

	for (ParamListPtr it = m_ParamList.begin();it!=m_ParamList.end();it++)
	{
		if ( CStringA(it->strParamName).MakeLower() == strName )
		{
			it->strParamValue = strValue;
			bRes = TRUE;
			break;
		}
	}

	return bRes;;
}
BOOL CUrlParser::AddOrSetParamValue(CStringA strName,CStringA strValue)
{
	BOOL bRes = FALSE;
	if (strName.GetLength() == 0)
	{
		return bRes;
	}
	CStringA strTempValue;
	BOOL bFoundValue = GetParamValueByName(strName,strTempValue);
	if ( FALSE == bFoundValue )
	{
		ParamNode node;
		node.strParamName = strName;
		node.strParamValue = strValue;

		int nCount = m_ParamList.size();
		m_ParamList.push_back(node);

		bRes = ( m_ParamList.size() - nCount ) == 1;
	}
	else
	{
		bRes = SetParamValueByName(strName,strValue);
	}
	return bRes;

}
CStringA CUrlParser::BuildUrl()
{
	CStringA strNewUrl;
	strNewUrl = m_strProtocol+"://"+m_strHost+m_strPath;
	
	if (m_ParamList.size() > 0)
	{
		strNewUrl+="?";
	}

	for (ParamListPtr it = m_ParamList.begin();it!=m_ParamList.end();it++)
	{
		strNewUrl+=it->strParamName+"="+it->strParamValue+"&";
	}
	strNewUrl.Delete(strNewUrl.GetLength()-1);

	return strNewUrl;
}
CStringA CUrlParser::BuildPath()
{
	CStringA strNewUrl;
	strNewUrl = m_strPath;

	if (m_ParamList.size() > 0)
	{
		strNewUrl+="?";
	}

	for (ParamListPtr it = m_ParamList.begin();it!=m_ParamList.end();it++)
	{
		if (it->strParamValue.GetLength() == 0)
		{
			strNewUrl+=it->strParamName+"&";
		}
		else
		{
			strNewUrl+=it->strParamName+"="+it->strParamValue+"&";
		}
		
	}

	strNewUrl.Delete(strNewUrl.GetLength()-1);

	return strNewUrl;
}