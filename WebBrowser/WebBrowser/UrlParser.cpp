#include "StdAfx.h"
#include "HelpFun.h"
#include "UrlParser.h"

CUrlParser::CUrlParser(void)
{

}

CUrlParser::~CUrlParser(void)
{
}

BOOL CUrlParser::ParseUrl(LPCSTR pchUrl)
{
	m_strUrl = pchUrl;

	CStringA strTempStrs[2];
	DivisionStringA("?",m_strUrl,strTempStrs,2);
	
	CStringA strTempStrs1[2];
	DivisionStringA("://",strTempStrs[0],strTempStrs1,2);
	m_strProtocol = strTempStrs1[0];

	int nStart = strTempStrs1[1].Find("/");
	if (nStart > 0)
	{
		m_strHost = strTempStrs1[1].Left(nStart);
		m_strPath = strTempStrs1[1].Right(strTempStrs1[1].GetLength() - nStart);

		int nPathEnd = m_strPath.ReverseFind(L'/');
		if ( nPathEnd >= 0)
		{
			//get file name
			m_strFileName = m_strPath.Right(m_strPath.GetLength() - nPathEnd-1);

			if ( nPathEnd > 0 )
			{
				m_strPath = m_strPath.Left(nPathEnd);
			}
			else
			{
				m_strPath = "/";
			}
		}

	}
	CStringA strTempStrs3[30];
	int nParamCount = DivisionStringA("&",strTempStrs[1],strTempStrs3,30);
	for (int i=0;i<nParamCount;i++)
	{
		CStringA strTempStrs4[2];
		DivisionStringA("=",strTempStrs3[i],strTempStrs4,2);

		AddOrSetParamValue(strTempStrs4[0],strTempStrs4[1]);

	}
	return FALSE;
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

CStringA CUrlParser::GetProtocol()
{
	return m_strProtocol;
}

CStringA CUrlParser::GetDomain()
{
	return m_strHost;
}
CStringA CUrlParser::GetPath()
{
	return m_strPath;
}
CStringA CUrlParser::GetFileName()
{
	return m_strFileName;
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
	strNewUrl+=L"/";
	strNewUrl+=m_strFileName;

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