// 内存INI文件读取.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <atlstr.h>
#include <map>
class CMemIniFile
{
public:
	typedef std::map<CStringW,CStringW> MAP_DATA,*PMAP_DATA;
	typedef MAP_DATA::iterator MAP_DATA_PTR;

	typedef std::map<CStringW,MAP_DATA> MAP_SECTION,*PMAP_SECTIPN;
	typedef MAP_SECTION::iterator MAP_SECTION_PTR;

protected:
	
	MAP_SECTION m_mapSection;

public:
	CMemIniFile( ){}

	~CMemIniFile(){}
protected:
	BOOL ParseDataLineW( LPCWSTR pszData,LONGLONG llLen,LONGLONG &llLineStart,LONGLONG &llLineEnd )
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

	VOID ParseSectionDataW(LPCWSTR pszData,LONGLONG llLen,CString &strName,CString &strValue)
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

	BOOL ParseUnicodeStringW(LPCWSTR pszData,LONGLONG llLen)
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

public:
	BOOL ParseMemoryDataW( CONST BYTE *pData , int nDataLen )
	{
		LONGLONG llStrLen = nDataLen/sizeof(WCHAR);
		ParseUnicodeStringW((LPCWSTR)pData,llStrLen);
		return TRUE;
	}

	CString GetIniString(LPCWSTR pszSection,LPCWSTR pszKeyName,LPCWSTR pszDefaultValue)
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
	UINT GetIniUint(LPCWSTR pszSection,LPCWSTR pszKeyName,UINT unDefaultValue)
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

	UINT GetSectionCount()
	{
		return m_mapSection.size();
	}

	UINT GetSectionDataCount(LPCWSTR pszSection)
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

	CString EnumSectionByIndex( UINT nIndex )
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

#if defined(DEBUG) || defined(_DEBUG)
	VOID PrintTree()
	{
		for ( MAP_SECTION_PTR it = m_mapSection.begin();it!= m_mapSection.end();it++ )
		{
			OutputDebugStringW(L"["+it->first+L"]\r\n");
			for (MAP_DATA_PTR itdata = it->second.begin();itdata != it->second.end();itdata++)
			{
				OutputDebugStringW(itdata->first + L"=" + itdata->second+ L"\r\n");
			}
		}
	}
#endif
};

int _tmain(int argc, _TCHAR* argv[])
{
	
	LPCWSTR pszData=L"count=10\r\nname=gaozan\r\nhahah=hahahah";
	
	CMemIniFile MemIni;
	MemIni.ParseMemoryDataW((BYTE *)pszData,wcslen(pszData)*2);
	MemIni.PrintTree();
	CString strValue = MemIni.GetIniString(L"",L"count",L"aaaa");
	UINT nValue = MemIni.GetIniUint(L"",L"count",0);

	OutputDebugStringW(L"\r\n");

	int nIndex = 0;
	while (TRUE)
	{
		CString strSection = MemIni.EnumSectionByIndex(nIndex);

		if (strSection.GetLength() == 0 )
		{
			break;
		}

		OutputDebugStringW(strSection+L"\r\n");

		nIndex++;
	}
	

	return 0;
}

