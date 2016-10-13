#include "StdAfx.h"
#include "HttpRecvParser.h"

int DivisionString(CStringA strSeparate, CStringA strSourceString, CStringA * pStringArray, int nArrayCount)
{

	if(
		strSeparate.GetLength() == 0 ||
		strSourceString.GetLength() == 0||
		pStringArray == NULL || 
		nArrayCount <=1 
		)
	{
		return 0;
	}

	int nCount = 0;
	while(true)
	{
		int nEnd = strSourceString.Find(strSeparate,0);
		if ( nEnd >= 0 )
		{
			pStringArray[nCount] = strSourceString.Left(nEnd);
			nCount++;

			strSourceString = strSourceString.Right(strSourceString.GetLength() - nEnd-strSeparate.GetLength());
		}
		else
		{
			pStringArray[nCount] = strSourceString;
			nCount++;
			strSourceString = "";

			break;
		}

		if ( nCount >= nArrayCount)
		{
			break;
		}
	}

	return nCount;
}
#define MAX_ACCEPT_HEAD_LEN 100

CHttpRecvParser::CHttpRecvParser(void)
{
	m_bResponseHead = FALSE;
	m_ContentStart = -1;
}

CHttpRecvParser::~CHttpRecvParser(void)
{
}

BOOL CHttpRecvParser::ParseData(const char * pBuffer,int nLen)
{
	BOOL bRes = FALSE;
	if (pBuffer == NULL || nLen < 50 )
	{
		return bRes;
	}



	CStringA strData;
	strData = pBuffer;

	CStringA strHttpHeader;
	int HttpHeadEnd = strData.Find("\r\n\r\n");
	if ( HttpHeadEnd > 0 )
	{
		char chHeadData[9];
		memcpy_s(chHeadData,8,pBuffer,8);
		chHeadData[8] = 0;

		
		if( StrCmpIA(chHeadData,"http/1.1") == 0 || StrCmpIA(chHeadData,"http/1.0") == 0 )
		{
			m_bResponseHead = TRUE;
			m_ContentStart = HttpHeadEnd+4;
			bRes = TRUE;
		}
		else
		{
			return FALSE;
		}

		strHttpHeader = strData.Left(HttpHeadEnd);

		CStringA strHeaders[MAX_ACCEPT_HEAD_LEN];
		int nCount = DivisionString("\r\n",strHttpHeader,strHeaders,MAX_ACCEPT_HEAD_LEN);
		if (nCount >= 1)
		{
			CStringA strTempStrs[10];
			int nTempCount = DivisionString(" ",strHeaders[0],strTempStrs,10);
			m_strHttpVersion = strTempStrs[0];
			m_strResponseCode = strTempStrs[1];

			for (int i=2;i<nTempCount;i++)
			{
				m_strResponseText += strTempStrs[i]+" ";
			}
			m_strResponseText.TrimRight();
			

			for(int i = 1;i<nCount;i++)
			{
				CStringA strHeader;
				CStringA strValue;
				int nHeadEnd = strHeaders[i].Find(":");
				if ( nHeadEnd > 0 )
				{
					strHeader = strHeaders[i].Left(nHeadEnd);
					strValue = strHeaders[i].Right(strHeaders[i].GetLength() - nHeadEnd - 1);
					strValue.Trim();
					AddResponseNode(strHeader,strValue);
				}

				
			}
			
		}
	}
	else
	{
		
	}

	return bRes;
}

BOOL CHttpRecvParser::IsResponseHead()
{
	return m_bResponseHead;
}

LONG CHttpRecvParser::GetContentStart()
{
	return m_ContentStart;
}
CStringA CHttpRecvParser::GetResponseCode()
{
	return m_strResponseCode;
}
CStringA CHttpRecvParser::GetValueByName(CStringA strName)
{
	CStringA strValue;

	if ( strName.GetLength() == 0 )
	{
		return strValue;
	}

	strName.MakeLower();


	for (RESPONSE_LIST_PTR it = m_ResponseList.begin();it!=m_ResponseList.end();it++)
	{
		if (CStringA(it->strResponseName).MakeLower() == strName)
		{
			strValue = it->strResponseValue;
			break;
		}
	}

	return strValue;
}
BOOL CHttpRecvParser::SetValueByName(CStringA strName,CStringA strValue)
{
	BOOL bRes = FALSE;
	if ( strName.GetLength() == 0 )
	{
		return bRes;
	}
	strName.MakeLower();

	for (RESPONSE_LIST_PTR it = m_ResponseList.begin();it!=m_ResponseList.end();it++)
	{
		if (CStringA(it->strResponseName).MakeLower() == strName)
		{
			it->strResponseValue = strValue;
			bRes = TRUE;
			break;
		}
	}
	return bRes;
}
BOOL CHttpRecvParser::BuildBuffer(CStringA &strNewBuffer)
{
	strNewBuffer = m_strHttpVersion+" ";
	strNewBuffer+= m_strResponseCode+" ";
	strNewBuffer+= m_strResponseText+"\r\n";

	for (RESPONSE_LIST_PTR it = m_ResponseList.begin();it!=m_ResponseList.end();it++)
	{
		strNewBuffer+=it->strResponseName+": "+it->strResponseValue+"\r\n";
	}

	strNewBuffer+="\r\n";
// #ifdef DEBUG
// 	OutputDebugStringA("ÖØ½¨»º³åÇø\n");
// 	OutputDebugStringA(strNewBuffer);
// #endif
	return TRUE;
}
BOOL CHttpRecvParser::AddResponseNode(CStringA strName,CStringA strValue)
{
	BOOL bRes = FALSE;
	if (strName.GetLength() == 0)
	{
		return bRes;
	}

	RESPONSE_NODE node;
	node.strResponseName = strName;
	node.strResponseValue = strValue;

	int nCount = m_ResponseList.size();
	m_ResponseList.push_back(node);

	return (m_ResponseList.size()-nCount) == 1 ;
}


BOOL CHttpRecvParser::DelResponseNode(CStringA strName)
{
	BOOL bRes = FALSE;
	if (strName.GetBuffer() == 0)
	{
		return bRes;
	}

	for (RESPONSE_LIST_PTR it = m_ResponseList.begin();it!=m_ResponseList.end();it++)
	{
		if (it->strResponseName.CompareNoCase(strName) == 0)
		{
			m_ResponseList.erase(it);
			bRes = TRUE;
			break;
		}
	}

	return bRes;
}