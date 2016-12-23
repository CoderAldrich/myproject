// 内存INI文件读取.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MemIni.h"


BOOL CMemIniFile::ParseDataLineW( LPCWSTR pszData,LONGLONG llLen,LONGLONG &llLineStart,LONGLONG &llLineEnd )
{
	llLineStart = 0;
	llLineEnd = 0;

	LONGLONG llIndex = 0;
	while( llIndex < llLen && (pszData[llIndex] == L'\r'|| pszData[llIndex] == L'\n') ) llIndex++;

	if ( llIndex >= llLen )
	{
		return FALSE;
	}

	llLineStart = llIndex;
	llLineEnd = llLen-1;

	llIndex++;

	while( llIndex < llLen && (pszData[llIndex] != L'\r' &&  pszData[llIndex] != L'\n')  ) llIndex++;

	if ( llIndex >=  llLen )
	{
		llLineEnd = llIndex;
	}
	else
	{
		llLineEnd = llIndex-1;
	}

	return TRUE;
}

VOID CMemIniFile::ParseSectionDataW(LPCWSTR pszData,LONGLONG llLen,CString &strName,CString &strValue)
{
	CString strTemp;
	strTemp = pszData;
	int nTag = strTemp.Find(L"=",1);
	if ( nTag > 0 )
	{
		strName = strTemp.Left(nTag);
		strValue = strTemp.Right(strTemp.GetLength() - nTag -1 );
	}
}

BOOL CMemIniFile::ParseUnicodeStringW(LPCWSTR pszData,LONGLONG llLen)
{
	CString strTempSection;

	LONGLONG llIndex = 0;

	while ( llIndex < llLen )
	{
		LONGLONG llLineStart = 0;
		LONGLONG llLineEnd = 0;
		ParseDataLineW(pszData+llIndex,llLen - llIndex,llLineStart,llLineEnd);

		CString strTempLine;

		for (LONGLONG i = llLineStart ; i <= llLineEnd ;i++)
		{
			strTempLine += (WCHAR)pszData[llIndex+i];
		}

		strTempLine.Trim();

		//是节的开始
		if ( strTempLine.GetAt(0) == L'[' )
		{
			int nSectionEnd = strTempLine.Find(L"]");
			if ( nSectionEnd > 0 )
			{
				strTempSection = strTempLine.Mid(1,nSectionEnd-1);
				strTempSection.Trim();
			}
		}
		else
		{
			CString strTempName;
			CString strTempValue;

			ParseSectionDataW(strTempLine,strTempLine.GetLength(),strTempName,strTempValue);

			strTempName.Trim();
			strTempValue.Trim();

			if ( strTempName.GetLength() > 0 )
			{
				MAP_SECTION_PTR it = m_mapSection.find(strTempSection);
				if (it == m_mapSection.end())
				{
					MAP_DATA Data;
					Data.insert(std::make_pair(strTempName,strTempValue));
					m_mapSection.insert(std::make_pair(strTempSection,Data));
				}
				else
				{
					it->second.insert(std::make_pair(strTempName,strTempValue));
				}
			}
		}

		llIndex+=(llLineEnd+1);

	}

	return FALSE;
}


BOOL CMemIniFile::ParseMemoryDataW( CONST BYTE *pData , int nDataLen )
{
	LONGLONG llStrLen = nDataLen/sizeof(WCHAR);

	CString strData;
	strData.Append((WCHAR *)pData,llStrLen);

	ParseUnicodeStringW((LPCWSTR)strData,llStrLen);
	return TRUE;
}

CString CMemIniFile::GetIniString(LPCWSTR pszSection,LPCWSTR pszKeyName,LPCWSTR pszDefaultValue)
{
	CString strKeyValue = pszDefaultValue;

	for ( MAP_SECTION_PTR it = m_mapSection.begin();it!= m_mapSection.end();it++ )
	{
		if (it->first.CompareNoCase(pszSection) == 0)
		{
			for (MAP_DATA_PTR itdata = it->second.begin();itdata != it->second.end();itdata++)
			{
				if ( itdata->first.CompareNoCase(pszKeyName) == 0)
				{
					strKeyValue = itdata->second;
					break;
				}
			}

			break;
		}
	}

	return strKeyValue;
}


UINT CMemIniFile::GetIniUint(LPCWSTR pszSection,LPCWSTR pszKeyName,UINT unDefaultValue)
{
	UINT unValue = unDefaultValue;

	CString strTemp = GetIniString(pszSection,pszKeyName,L"");
	if ( strTemp.GetLength() > 0 )
	{
		BOOL bNumber = TRUE;
		for(int i=0;i<strTemp.GetLength();i++)
		{
			WCHAR wchTemp = strTemp.GetAt(i);
			if ( !(wchTemp <=L'9' && wchTemp >= L'0'))
			{
				bNumber = FALSE;
				break;
			}
		}

		if (bNumber)
		{
			unValue = _ttoi(strTemp);
		}
	}

	return unValue;
}



UINT CMemIniFile::GetSectionCount()
{
	return m_mapSection.size();
}

UINT CMemIniFile::GetSectionDataCount(LPCWSTR pszSection)
{
	for ( MAP_SECTION_PTR it = m_mapSection.begin();it!= m_mapSection.end() ;it++ )
	{
		if (it->first.CompareNoCase(pszSection) == 0)
		{
			return it->second.size();
		}
	}

	return 0;
}

CString CMemIniFile::EnumSectionByIndex( UINT nIndex )
{
	CString strSection;
	UINT i=0;
	for ( MAP_SECTION_PTR it = m_mapSection.begin();it!= m_mapSection.end() ;it++ )
	{
		if ( i == nIndex )
		{
			strSection = it->first;
			break;
		}
		i++;
	}
	return strSection;
}

BOOL CMemIniFile::WriteIniString( LPCWSTR pszSection,LPCWSTR pszKeyName,LPCWSTR pszKeyValue )
{
	MAP_SECTION_PTR it = m_mapSection.find(pszSection);
	if (it == m_mapSection.end())
	{
		MAP_DATA Data;
		Data.insert(std::make_pair(pszKeyName,pszKeyValue));
		m_mapSection.insert(std::make_pair(pszSection,Data));
	}
	else
	{
		MAP_DATA_PTR datait = it->second.find(pszKeyName);
		if (datait == it->second.end())
		{
			it->second.insert(std::make_pair(pszKeyName,pszKeyValue));
		}
		else
		{
			datait->second = pszKeyValue;
		}
	}

	return TRUE;
}

BOOL CMemIniFile::WriteIniInt( LPCWSTR pszSection,LPCWSTR pszKeyName,int nValue )
{
	CString strKeyValue;
	strKeyValue.Format(L"%d",nValue);

	return WriteIniString(pszSection,pszKeyName,strKeyValue);
}

CString CMemIniFile::BuildData()
{
	CString strMemIniData;

	for ( MAP_SECTION_PTR it = m_mapSection.begin();it!= m_mapSection.end();it++ )
	{
		strMemIniData+=L"["+it->first+L"]\r\n";
		for (MAP_DATA_PTR itdata = it->second.begin();itdata != it->second.end();itdata++)
		{
			strMemIniData+=itdata->first + L"=" + itdata->second+ L"\r\n";
		}
	}

	return strMemIniData;
}
