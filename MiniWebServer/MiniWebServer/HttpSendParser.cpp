#include "StdAfx.h"
#include "HttpSendParser.h"
#include "HelpFun.h"

CHttpSendParser::CHttpSendParser(void)
{
	m_ContentStart = -1;
	m_bHaveProxy = FALSE;
}

CHttpSendParser::~CHttpSendParser(void)
{

}


BOOL CHttpSendParser::ParseData(const char * pBuffer,int nLen)
{

	if ( NULL == pBuffer || nLen < 20 )
	{
		return FALSE;
	}


	if ( 
		!(
		(pBuffer[0] == L'G' && pBuffer[1] == L'E' && pBuffer[2] == L'T' && pBuffer[3] == L' ') 
		||
		(pBuffer[0] == L'P' && pBuffer[1] == L'O' && pBuffer[2] == L'S' && pBuffer[3] == L'T' && pBuffer[4] == L' ' )
		)
		)
	{
		return FALSE;
	}

	if ( !(pBuffer[nLen - 1] == L'\n' && pBuffer[nLen - 2] == L'\r' && pBuffer[nLen - 3] == L'\n' && pBuffer[nLen - 4] == L'\r'))
	{
		return FALSE;
	}

	

	BOOL bRes = FALSE;
	CStringA strData;
	strData = pBuffer;

	CStringA strHttpHeader;
	int HttpHeadEnd = strData.Find("\r\n\r\n");
	if ( HttpHeadEnd > 0 )
	{
		bRes = TRUE;

		m_ContentStart = HttpHeadEnd+4;
		
		strHttpHeader = strData.Left(HttpHeadEnd);
		
		CStringA strHeaders[20];
		int count = DivisionString("\r\n",strHttpHeader,strHeaders,20);
		if (count >= 1)
		{
			CStringA strTempStrs[10];
			DivisionString(" ",strHeaders[0],strTempStrs,10);
			m_strRequestMeth = strTempStrs[0];
			m_strRequestPath = strTempStrs[1];
			m_strRequestVersion = strTempStrs[2];

			//判断是不是发给代理服务器数据包
			if (m_strRequestPath.Left(1)!=L"/")
			{
				m_bHaveProxy = TRUE;
				int nRealPathStart =   m_strRequestPath.Find("/",9/*"http://" 的长度*/);
				if (nRealPathStart > 0)
				{
					m_strRequestPath = m_strRequestPath.Right(m_strRequestPath.GetLength() - nRealPathStart);
				}
			}
			for(int i = 1;i<count;i++)
			{
				DivisionString(": ",strHeaders[i],strTempStrs,2);
				
				AddRequestNode(strTempStrs[0],strTempStrs[1]);

				if (CStringA(strTempStrs[0]).MakeLower() == "host")
				{
					m_strRequestHost=strTempStrs[1];
				}
			}

		}
	}

	return bRes;
}


LONG CHttpSendParser::GetContentStart()
{
	return m_ContentStart;
}

CStringA CHttpSendParser::GetParseUrl(void)
{
	return "http://"+m_strRequestHost+m_strRequestPath;
}
CStringA CHttpSendParser::GetRequestMeth()
{
	return m_strRequestMeth;
}

BOOL CHttpSendParser::GetHaveProxy()
{
	return m_bHaveProxy;
}
CStringA CHttpSendParser::GetValueByName(CStringA strName)
{
	CStringA strValue;

	if ( strName.GetLength() == 0 )
	{
		return strValue;
	}

	strName.MakeLower();


	for (REQUEST_LIST_PTR it = m_RequestList.begin();it!=m_RequestList.end();it++)
	{
		if (CStringA(it->strRequestName).MakeLower() == strName)
		{
			strValue = it->strRequestValue;
			break;
		}
	}

	return strValue;
}

BOOL CHttpSendParser::SetValueByName(CStringA strName,CStringA strValue)
{
	BOOL bRes = FALSE;
	if ( strName.GetLength() == 0 )
	{
		return bRes;
	}
	strName.MakeLower();

	for (REQUEST_LIST_PTR it = m_RequestList.begin();it!=m_RequestList.end();it++)
	{
		if (CStringA(it->strRequestName).MakeLower() == strName)
		{
			if (strName == "host")
			{
				m_strRequestHost=strValue;
			}

			it->strRequestValue = strValue;
			bRes = TRUE;
			break;
		}
	}
	return bRes;
}

BOOL CHttpSendParser::SetPath(CStringA strNewPath)
{
	if (strNewPath.GetLength() == 0)
	{
		return FALSE;
	}

	if (strNewPath.GetAt(0) !=  '/')
	{
		return FALSE;
	}

	m_strRequestPath = strNewPath;

	return TRUE;
}

CStringA CHttpSendParser::GetHost(void)
{
	return m_strRequestHost;
}

BOOL CHttpSendParser::BuildBuffer(CStringA &strNewBuffer)
{

	strNewBuffer = m_strRequestMeth+" ";

	if (m_bHaveProxy)
	{
		strNewBuffer+= "http://"+m_strRequestHost+m_strRequestPath+" ";
	}
	else
	{
		strNewBuffer+= m_strRequestPath+" ";
	}
	
	strNewBuffer+= m_strRequestVersion+"\r\n";
	
	for (REQUEST_LIST_PTR it = m_RequestList.begin();it!=m_RequestList.end();it++)
	{
		strNewBuffer+=it->strRequestName+": "+it->strRequestValue+"\r\n";
	}

	strNewBuffer+="\r\n";
// #ifdef DEBUG
// 	OutputDebugStringA("重建缓冲区\n");
// 	OutputDebugStringA(strNewBuffer);
// #endif
	return true;
}
BOOL CHttpSendParser::AddRequestNode(CStringA strName,CStringA strValue)
{
	if (strName.GetBuffer() == 0)
	{
		return FALSE;
	}

	REQUEST_NODE node;
	node.strRequestName = strName;
	node.strRequestValue = strValue;
	
	int nCount = m_RequestList.size();
	m_RequestList.push_back(node);

	return (m_RequestList.size()-nCount) == 1 ;

}

BOOL CHttpSendParser::DelRequestNode(CStringA strName)
{
	if (strName.GetBuffer() == 0)
	{
		return FALSE;
	}

	BOOL bDel = FALSE;
	for (REQUEST_LIST_PTR it = m_RequestList.begin();it!=m_RequestList.end();it++)
	{
		if(it->strRequestName.CompareNoCase(strName) == 0)
		{
			m_RequestList.erase(it);
			bDel = TRUE;
			break;
		}
	}
	return bDel;

}

BOOL CHttpSendParser::SetHttpVersion(CStringA strHttpVersion)
{
	m_strRequestVersion = strHttpVersion;
	return TRUE;
}