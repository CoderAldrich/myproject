#include "stdafx.h"
#include "ProtocolHandler.h"

CProtocolHandler::CProtocolHandler()
{

}
CProtocolHandler::~CProtocolHandler()
{

}

BOOL CProtocolHandler::ParseProtocolString( LPCSTR pchDataBuffer,int nDataLen )
{
	CStringA strDataBuffer;
	strDataBuffer.Append(pchDataBuffer,nDataLen);

	CStringA strLineData;
	int curPos = 0;
	strLineData= strDataBuffer.Tokenize("\r\n",curPos);
	while (strLineData != "")
	{
		CStringA strKeyName;
		CStringA strKeyValue;

		int nNameEnd = strLineData.Find("=");
		if ( nNameEnd > 0 )
		{
			strKeyName = strLineData.Left( nNameEnd );
			strKeyValue = strLineData.Right( strLineData.GetLength() - nNameEnd - 1 );

			strKeyName.Trim();
			strKeyName.MakeLower();
		}

		if ( strKeyName.GetLength() > 0 )
		{
			MAP_DATA_PTR it = m_mapData.find(strKeyName);
			if (it == m_mapData.end())
			{
				m_mapData.insert(std::make_pair(strKeyName,strKeyValue));
			}
		}

		strLineData = strDataBuffer.Tokenize("\r\n", curPos);
	};

	return TRUE;
}

CStringA CProtocolHandler::GetParamValueString( LPCSTR pchKeyName,LPCSTR pchDefaultValue )
{
	CStringA strValue;
	strValue = pchDefaultValue;

	MAP_DATA_PTR it = m_mapData.find(CStringA(pchKeyName).MakeLower());
	if (it != m_mapData.end())
	{
		strValue = it->second;
	}

	return strValue;
}

int CProtocolHandler::GetParamValueInt( LPCSTR pchKeyName,int nDefaultValue )
{
	int nValue;
	nValue = nDefaultValue;

	CStringA strTempValue;
	strTempValue = GetParamValueString(CStringA(pchKeyName).MakeLower(),"");
	if ( strTempValue.GetLength() > 0 )
	{
		nValue = atoi(strTempValue);
	}

	return nValue;
}

VOID CProtocolHandler::SetParamValueString( LPCSTR pchKeyName,LPCSTR pchValue )
{
	CStringA strKeyName;
	strKeyName = pchKeyName;
	strKeyName.MakeLower();

	MAP_DATA_PTR it = m_mapData.find(strKeyName);
	if (it == m_mapData.end())
	{
		m_mapData.insert(std::make_pair(strKeyName,pchValue));
	}
	else
	{
		it->second = pchValue;
	}

}

VOID CProtocolHandler::SetParamValueInt( LPCSTR pchKeyName, int nValue )
{
	CStringA strIntValue;
	strIntValue.Format("%d",nValue);
	SetParamValueString( pchKeyName,strIntValue );
}

CStringA CProtocolHandler::BuildData()
{
	CStringA strData;

	for ( MAP_DATA_PTR it = m_mapData.begin();it!=m_mapData.end();it++ )
	{
		strData+=it->first;
		strData+="=";
		strData+=it->second;
		strData+="\r\n";
	}

	return strData;
}